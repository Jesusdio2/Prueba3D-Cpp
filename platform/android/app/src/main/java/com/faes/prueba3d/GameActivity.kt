package com.faes.prueba3d

import android.app.Activity
import android.app.UiModeManager
import android.content.Context
import android.content.res.Configuration
import android.os.Build
import android.os.Bundle
import android.util.Log
import android.view.InputDevice
import android.view.KeyEvent
import android.view.MotionEvent
import android.view.WindowManager
import androidx.core.view.WindowCompat
import androidx.core.view.WindowInsetsCompat
import androidx.core.view.WindowInsetsControllerCompat

class GameActivity : Activity() {

    private lateinit var gameSurface: GameSurface
    
    private var deviceType = DEVICE_PHONE
    private var isEmulator = false

    companion object {
        const val DEVICE_PHONE = 0
        const val DEVICE_WATCH = 1
        const val DEVICE_TV = 2
        const val DEVICE_XR = 3
        const val DEVICE_AUTOMOTIVE = 4
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        try {
            System.loadLibrary("prueba3d_native")
        } catch (e: UnsatisfiedLinkError) {
            Log.e("GameActivity", "Fallo nativo crítico: ${e.message}")
            finishAffinity()
            return
        }

        detectDeviceEnvironment()
        configureWindowSystem()

        gameSurface = GameSurface(this)
        setContentView(gameSurface)
        
        NativeBridge.setDeviceType(deviceType, isEmulator)
    }

    private fun detectDeviceEnvironment() {
        val uiModeManager = getSystemService(Context.UI_MODE_SERVICE) as UiModeManager
        val modeType = uiModeManager.currentModeType

        deviceType = when (modeType) {
            Configuration.UI_MODE_TYPE_WATCH -> DEVICE_WATCH
            Configuration.UI_MODE_TYPE_TELEVISION -> DEVICE_TV
            Configuration.UI_MODE_TYPE_VR_HEADSET -> DEVICE_XR
            Configuration.UI_MODE_TYPE_CAR -> DEVICE_AUTOMOTIVE
            else -> DEVICE_PHONE
        }

        if (Build.MANUFACTURER.contains("Meta", ignoreCase = true) || 
            Build.BRAND.contains("Pico", ignoreCase = true)) {
            deviceType = DEVICE_XR
        }

        isEmulator = (Build.HARDWARE.contains("goldfish") || 
                      Build.HARDWARE.contains("ranchu") || 
                      Build.MODEL.contains("google_sdk") || 
                      Build.FINGERPRINT.startsWith("generic"))
                      
        Log.i("GameActivity", "Entorno Detectado: Tipo=$deviceType | EsEmulador=$isEmulator")
    }

    private fun configureWindowSystem() {
        window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON)
        WindowCompat.setDecorFitsSystemWindows(window, false)
        val controller = WindowCompat.getInsetsController(window, window.decorView)
        
        when (deviceType) {
            DEVICE_WATCH -> controller.hide(WindowInsetsCompat.Type.systemBars())
            DEVICE_TV, DEVICE_XR -> {
                controller.hide(WindowInsetsCompat.Type.systemBars())
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                    window.attributes.layoutInDisplayCutoutMode = 
                        WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES
                }
            }
            else -> {
                controller.hide(WindowInsetsCompat.Type.systemBars())
                controller.systemBarsBehavior = WindowInsetsControllerCompat.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                    window.attributes.layoutInDisplayCutoutMode = 
                        WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES
                }
            }
        }
    }

    override fun onKeyDown(keyCode: Int, event: KeyEvent?): Boolean {
        val device = if (event?.source?.and(InputDevice.SOURCE_GAMEPAD) == InputDevice.SOURCE_GAMEPAD) {
            NativeBridge.DEVICE_GAMEPAD
        } else {
            NativeBridge.DEVICE_KEYBOARD
        }

        NativeBridge.onInputEvent(
            device,
            NativeBridge.ACTION_DOWN,
            keyCode,
            0f, 0f, 0f, 0f
        )
        return true
    }

    override fun onKeyUp(keyCode: Int, event: KeyEvent?): Boolean {
        val device = if (event?.source?.and(InputDevice.SOURCE_GAMEPAD) == InputDevice.SOURCE_GAMEPAD) {
            NativeBridge.DEVICE_GAMEPAD
        } else {
            NativeBridge.DEVICE_KEYBOARD
        }

        NativeBridge.onInputEvent(
            device,
            NativeBridge.ACTION_UP,
            keyCode,
            0f, 0f, 0f, 0f
        )
        return true
    }

    override fun onGenericMotionEvent(event: MotionEvent?): Boolean {
        if (event == null) return false

        // 1. Manejo de Sticks de Gamepad
        if (event.source and InputDevice.SOURCE_JOYSTICK == InputDevice.SOURCE_JOYSTICK &&
            event.action == MotionEvent.ACTION_MOVE) {
            
            NativeBridge.onInputEvent(
                NativeBridge.DEVICE_GAMEPAD,
                NativeBridge.ACTION_AXIS,
                0,
                0f, 0f,
                event.getAxisValue(MotionEvent.AXIS_X),
                event.getAxisValue(MotionEvent.AXIS_Y)
            )
            return true
        }

        // 2. Manejo de Mouse (Scroll Wheel y Botones Extra)
        if (event.source and InputDevice.SOURCE_MOUSE == InputDevice.SOURCE_MOUSE) {
            if (event.action == MotionEvent.ACTION_SCROLL) {
                NativeBridge.onInputEvent(
                    NativeBridge.DEVICE_MOUSE,
                    NativeBridge.ACTION_AXIS,
                    0,
                    event.x, event.y,
                    0f,
                    event.getAxisValue(MotionEvent.AXIS_VSCROLL)
                )
                return true
            }
        }
        return super.onGenericMotionEvent(event)
    }

    override fun onPause() {
        super.onPause()
        NativeBridge.onPause()
    }

    override fun onResume() {
        super.onResume()
        NativeBridge.onResume()
    }
}
