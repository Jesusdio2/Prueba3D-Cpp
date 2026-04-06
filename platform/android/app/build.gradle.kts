plugins {
    id("com.android.application")
    kotlin("android") // No declarar versión aquí
}

android {
    namespace = "com.faes.prueba3d"
    compileSdk = 34

    defaultConfig {
        applicationId = "com.faes.prueba3d"
        minSdk = 24
        targetSdk = 34
        versionCode = 1
        versionName = "1.0"

        externalNativeBuild {
            cmake {
                cppFlags += "-std=c++17"
            }
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = false
        }
        debug {
            // Opcional: habilitar logs y depuración
            isDebuggable = true
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }

    kotlinOptions {
        jvmTarget = "17"
    }

    externalNativeBuild {
        cmake {
            // Ruta relativa a tu CMakeLists.txt de core/
            path = file("CMakeLists.txt")
        }
    }

    // Opcional: permite usar Jetpack Compose si agregas UI
    buildFeatures {
        compose = true
    }

    composeOptions {
        kotlinCompilerExtensionVersion = "1.5.3" // compatible con Compose Material3
    }
}

repositories {
    google()
    mavenCentral()
}

dependencies {
    implementation("androidx.core:core-ktx:1.12.0")
    implementation("androidx.activity:activity-compose:1.9.0")
    implementation("androidx.compose.material3:material3:1.2.0")
}