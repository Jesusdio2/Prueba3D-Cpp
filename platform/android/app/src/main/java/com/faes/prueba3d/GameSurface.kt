package com.faes.prueba3d

import android.content.Context
import android.opengl.GLSurfaceView
import android.util.AttributeSet
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class GameSurface @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null
) : GLSurfaceView(context, attrs) {

    private val renderer: NativeRenderer

    init {
        setEGLContextClientVersion(3) // OpenGL ES 3.0

        renderer = NativeRenderer()

        setRenderer(renderer)
        renderMode = RENDERMODE_CONTINUOUSLY
    }

    private class NativeRenderer : Renderer {

        external fun initGame()
        external fun updateGame(deltaTime: Float)

        private var lastTime = System.nanoTime()

        override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
            initGame()
        }

        override fun onDrawFrame(gl: GL10?) {
            val now = System.nanoTime()
            val delta = (now - lastTime) / 1_000_000_000f
            lastTime = now

            updateGame(delta)
        }

        override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
            // Luego lo mandamos a C++ si quieres viewport dinámico
        }
    }
}