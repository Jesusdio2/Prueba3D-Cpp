package com.faes.prueba3d

import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.material3.MaterialTheme
import androidx.compose.ui.viewinterop.AndroidView
import androidx.core.view.WindowCompat
import androidx.core.view.WindowInsetsCompat
import androidx.core.view.WindowInsetsControllerCompat
import android.view.WindowManager
import androidx.compose.ui.Modifier
import androidx.compose.foundation.layout.fillMaxSize

class GameActivity : ComponentActivity() {

    companion object {
        init {
            try {
                System.loadLibrary("prueba3d_native")
                Log.i("GameActivity", "Librería nativa cargada correctamente")
            } catch (e: UnsatisfiedLinkError) {
                Log.e("GameActivity", "No se encontró la librería nativa: ${e.message}")
            }
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // 1. Activar Edge-to-Edge (OBLIGATORIO en API 36)
        WindowCompat.setDecorFitsSystemWindows(window, false)

        // 2. Configurar modo inmersivo real para juegos
        val controller = WindowCompat.getInsetsController(window, window.decorView)
        controller.hide(WindowInsetsCompat.Type.systemBars()) // Esconde barras de estado y navegación
        controller.systemBarsBehavior =
            WindowInsetsControllerCompat.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE

        // 3. Usar el área de la cámara/notch (Solo para API 28+)
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.P) {
            window.attributes.layoutInDisplayCutoutMode =
                WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES
        }

        setContent {
            MaterialTheme {
                // Usamos un Modifier para asegurar que el Surface ocupe toda la pantalla
                AndroidView(
                    modifier = Modifier.fillMaxSize(),

                    factory = { context ->
                        GameSurface(context)
                    }
                )
            }
        }
    }
}
