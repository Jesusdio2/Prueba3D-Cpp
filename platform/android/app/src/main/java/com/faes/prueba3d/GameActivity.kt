package com.faes.prueba3d

import android.app.Activity
import android.app.AlertDialog
import android.os.Bundle
import android.util.Log
import android.view.KeyEvent
import android.view.WindowManager
import androidx.core.view.WindowCompat
import androidx.core.view.WindowInsetsCompat
import androidx.core.view.WindowInsetsControllerCompat

class GameActivity : Activity() {

    private lateinit var gameSurface: GameSurface

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // 1. Intentar cargar la librería C++ (.so) inmediatamente
        try {
            System.loadLibrary("prueba3d_native")
            Log.i("GameActivity", "Librería nativa cargada correctamente")
        } catch (e: UnsatisfiedLinkError) {
            Log.e("GameActivity", "No se encontró la librería nativa: ${e.message}")

            // Mostrar la advertencia visual retrocompatible
            AlertDialog.Builder(this)
                .setTitle("Error Crítico")
                .setMessage("No se pudieron cargar los componentes gráficos de C++. La aplicación se cerrará.")
                .setCancelable(false)
                .setPositiveButton("Aceptar") { _, _ ->
                    finish()
                }
                .show()

            return
        }

        // 2. Configuración de pantalla completa (Inmersiva)
        WindowCompat.setDecorFitsSystemWindows(window, false)

        val controller = WindowCompat.getInsetsController(window, window.decorView)
        controller.hide(WindowInsetsCompat.Type.systemBars())
        controller.systemBarsBehavior =
            WindowInsetsControllerCompat.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE

        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.P) {
            window.attributes.layoutInDisplayCutoutMode =
                WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES
        }

        // 3. Inflar superficie
        gameSurface = GameSurface(this)
        setContentView(gameSurface)
    }

    override fun onKeyDown(keyCode: Int, event: KeyEvent?): Boolean {
        if (NativeBridge.onKeyEvent(keyCode, true)) return true
        return super.onKeyDown(keyCode, event)
    }

    override fun onKeyUp(keyCode: Int, event: KeyEvent?): Boolean {
        if (NativeBridge.onKeyEvent(keyCode, false)) return true
        return super.onKeyUp(keyCode, event)
    }
}
