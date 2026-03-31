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
                System.loadLibrary("milib")
                Log.i("GameActivity", "Librería nativa cargada correctamente")
            } catch (e: UnsatisfiedLinkError) {
                Log.e("GameActivity", "No se encontró la librería nativa: ${e.message}")
            } catch (e: Exception) {
                Log.e("GameActivity", "Error al cargar la librería: ${e.message}")
            }
        }
    }

    external fun initGame()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        nativeLoaded = try {
            initGame()
            true
        } catch (e: UnsatisfiedLinkError) {
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
        Text("Juego inicializado correctamente con C++")
    }
}

@Composable
fun MissingLibraryDialog() {
    AlertDialog(
        onDismissRequest = { /* cerrar diálogo */ },
        confirmButton = {
            TextButton(onClick = { /* cerrar app o continuar */ }) {
                Text("Aceptar")
            }
        },
        title = { Text("Error de librería") },
        text = { Text("No se encontró la librería nativa. La aplicación continuará sin funciones avanzadas.") }
    )
}
