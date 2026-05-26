package com.faes.prueba3d

import android.os.Bundle
import android.util.Log
import android.view.WindowManager
import androidx.appcompat.app.AppCompatActivity
import androidx.appcompat.app.AlertDialog
import androidx.core.view.WindowCompat
import androidx.core.view.WindowInsetsCompat
import androidx.core.view.WindowInsetsControllerCompat

class GameActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // 1. Intentar cargar la librería C++ (.so) inmediatamente
        try {
            System.loadLibrary("prueba3d_native")
            Log.i("GameActivity", "Librería nativa cargada correctamente")
        } catch (e: UnsatisfiedLinkError) {
            Log.e("GameActivity", "No se encontró la librería nativa: ${e.message}")

            // Mostrar la advertencia visual retrocompatible usando AppCompat
            AlertDialog.Builder(this)
                .setTitle("Error Crítico")
                .setMessage("No se pudieron cargar los componentes gráficos de C++. La aplicación se cerrará.")
                .setCancelable(false)
                .setPositiveButton("Aceptar") { _, _ ->
                    finish() // Cierra la actividad limpiamente
                }
                .show()

            return // Detiene la ejecución para no intentar renderizar nada
        }

        // 2. Configuración de pantalla completa (Inmersiva) si la librería cargó con éxito
        WindowCompat.setDecorFitsSystemWindows(window, false)

        val controller = WindowCompat.getInsetsController(window, window.decorView)
        controller.hide(WindowInsetsCompat.Type.systemBars())
        controller.systemBarsBehavior =
            WindowInsetsControllerCompat.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE

        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.P) {
            window.attributes.layoutInDisplayCutoutMode =
                WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES
        }

        // 3. Inflar directamente tu superficie nativa sin intermediarios de Compose
        val gameSurface = GameSurface(this)
        setContentView(gameSurface)
    }
}
