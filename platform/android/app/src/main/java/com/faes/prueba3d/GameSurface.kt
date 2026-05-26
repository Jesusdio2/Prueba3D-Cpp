package com.faes.prueba3d

import android.content.Context
import android.util.AttributeSet
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView

import android.view.MotionEvent

class GameSurface @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null
) : SurfaceView(context, attrs), SurfaceHolder.Callback {

    private var renderThread: Thread? = null
    private var running = false

    init {
        holder.addCallback(this)
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        NativeBridge.onTouch(event.x, event.y, event.action)
        return true
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
        // Implementar cambio de resolución si es necesario
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
