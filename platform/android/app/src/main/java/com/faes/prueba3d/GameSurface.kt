package com.faes.prueba3d

import android.content.Context
import android.util.AttributeSet
import android.view.InputDevice
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView

import android.view.MotionEvent
import android.view.KeyEvent

class GameSurface @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null
) : SurfaceView(context, attrs), SurfaceHolder.Callback {

    private var renderThread: Thread? = null
    private var running = false

    init {
        holder.addCallback(this)
        isFocusable = true
        isFocusableInTouchMode = true
        requestFocus()
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        val device = if (event.source and InputDevice.SOURCE_MOUSE == InputDevice.SOURCE_MOUSE) {
            NativeBridge.DEVICE_MOUSE
        } else {
            NativeBridge.DEVICE_TOUCH
        }

        val action = when (event.action) {
            MotionEvent.ACTION_DOWN -> NativeBridge.ACTION_DOWN
            MotionEvent.ACTION_UP -> NativeBridge.ACTION_UP
            else -> NativeBridge.ACTION_MOVE
        }

        // Detectar qué botón del mouse se pulsó (si aplica)
        val buttonState = event.buttonState
        val keyCode = when {
            buttonState and MotionEvent.BUTTON_PRIMARY != 0 -> 1
            buttonState and MotionEvent.BUTTON_SECONDARY != 0 -> 2
            buttonState and MotionEvent.BUTTON_TERTIARY != 0 -> 3
            else -> 0
        }

        NativeBridge.onInputEvent(
            device,
            action,
            keyCode,
            event.x,
            event.y,
            0f,
            0f
        )
        return true
    }

    override fun onKeyDown(keyCode: Int, event: KeyEvent?): Boolean {
        if (NativeBridge.onKeyEvent(keyCode, true)) return true
        return super.onKeyDown(keyCode, event)
    }

    override fun onKeyUp(keyCode: Int, event: KeyEvent?): Boolean {
        if (NativeBridge.onKeyEvent(keyCode, false)) return true
        return super.onKeyUp(keyCode, event)
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        running = true
        val assetManager = context.assets
        renderThread = Thread {
            val width = holder.surfaceFrame.width()
            val height = holder.surfaceFrame.height()
            (context as? android.app.Activity)?.let { activity ->
                NativeBridge.initGame(activity, holder.surface, assetManager, width, height)
            }
            
            var lastTime = System.nanoTime()
            while (running) {
                val now = System.nanoTime()
                val delta = (now - lastTime) / 1_000_000_000f
                lastTime = now
                
                NativeBridge.updateGame(delta)

                if (NativeBridge.shouldQuit()) {
                    (context as? android.app.Activity)?.finish()
                    break
                }
            }
            
            NativeBridge.shutdownGame()
        }.apply { name = "GameRenderThread" }
        renderThread?.start()
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
        NativeBridge.updateViewport(width, height)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        running = false
        try {
            renderThread?.join()
        } catch (e: InterruptedException) {
            e.printStackTrace()
        }
    }
}
