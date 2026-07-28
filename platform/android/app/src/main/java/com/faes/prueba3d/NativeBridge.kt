package com.faes.prueba3d

import android.view.MotionEvent
import android.view.Surface
import android.content.res.AssetManager
import android.app.Activity

object NativeBridge {
    // Tipos de Dispositivo (Sincronizados con C++)
    const val DEVICE_TOUCH = 0
    const val DEVICE_MOUSE = 1
    const val DEVICE_KEYBOARD = 2
    const val DEVICE_GAMEPAD = 3
    const val DEVICE_XR = 4

    // Acciones (Sincronizadas con C++)
    const val ACTION_DOWN = 0
    const val ACTION_UP = 1
    const val ACTION_MOVE = 2
    const val ACTION_AXIS = 3

    external fun initGame(activity: Activity, surface: Surface, assetManager: AssetManager, width: Int, height: Int)
    external fun updateViewport(width: Int, height: Int)
    external fun updateGame(deltaTime: Float)
    external fun shutdownGame()
    external fun setGameState(state: Int)
    
    // El nuevo sistema unificado
    external fun onInputEvent(device: Int, action: Int, keyCode: Int, x: Float, y: Float, axisX: Float, axisY: Float)
    
    // Compatibilidad (opcional, pueden llamar internamente a onInputEvent)
    external fun onTouch(x: Float, y: Float, action: Int)
    external fun onKeyEvent(keyCode: Int, isPressed: Boolean): Boolean
    external fun onGenericMotionEvent(event: MotionEvent): Boolean

    external fun setDeviceType(deviceType: Int, isEmulator: Boolean)
    external fun onPause()
    external fun onResume()
    external fun shouldQuit(): Boolean
}
