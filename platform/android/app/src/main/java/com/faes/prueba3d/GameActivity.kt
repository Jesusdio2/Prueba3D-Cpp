package com.faes.prueba3d

import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.material3.*
import androidx.compose.runtime.*

class GameActivity : ComponentActivity() {

    private var nativeLoaded = false

    companion object {
        init {
            try {
                // Usamos el nombre real de la librería nativa
                System.loadLibrary("prueba3d_native")
                Log.i("GameActivity", "Librería nativa cargada correctamente")
            } catch (e: UnsatisfiedLinkError) {
                Log.e("GameActivity", "No se encontró la librería nativa: ${e.message}")
            }
        }
    }

    // Nombre que el puente JNI (jni_bridge.cpp) espera
    external fun initGame()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        nativeLoaded = try {
            initGame()
            true
        } catch (e: Exception) {
            false
        }

        setContent {
            MaterialTheme {
                if (nativeLoaded) {
                    GameScreen()
                } else {
                    MissingLibraryDialog()
                }
            }
        }
    }
}

@Composable
fun GameScreen() {
    Surface {
        Text("Juego 3D en C++ inicializado correctamente!")
    }
}

@Composable
fun MissingLibraryDialog() {
    AlertDialog(
        onDismissRequest = { },
        confirmButton = {
            TextButton(onClick = { }) {
                Text("Cerrar")
            }
        },
        title = { Text("Error Nativo") },
        text = { Text("No se pudo conectar con el motor C++.") }
    )
}
