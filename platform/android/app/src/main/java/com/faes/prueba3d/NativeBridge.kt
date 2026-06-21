package com.faes.prueba3d

import android.view.Surface
import android.content.res.AssetManager
import android.app.Activity

object NativeBridge {
    external fun initGame(activity: Activity, surface: Surface, assetManager: AssetManager, width: Int, height: Int)
    external fun updateGame(deltaTime: Float)
    external fun shutdownGame()
    external fun setGameState(state: Int)
    external fun onTouch(x: Float, y: Float, action: Int)
    external fun onKeyEvent(keyCode: Int, isPressed: Boolean): Boolean
    external fun shouldQuit(): Boolean
}
