// platform/android/app/build.gradle.kts
plugins {
    id("com.android.application")
    id("org.jetbrains.kotlin.android")
}

android {
    namespace = "com.faes.prueba3d"
    compileSdk = 35

    defaultConfig {
        applicationId = "com.faes.prueba3d"
        minSdk = 21
        targetSdk = 35
        versionCode = 1
        versionName = "1.0"

        externalNativeBuild {
            cmake {
                cppFlags += "-std=c++20"
                cppFlags += "-DBX_CONFIG_DEBUG=1"
                arguments += "-DANDROID_STL=c++_shared"
                abiFilters += listOf("arm64-v8a", "x86_64")
            }
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            externalNativeBuild {
                cmake {
                    cppFlags += "-DBX_CONFIG_DEBUG=0"
                }
            }
        }
        debug {
            isDebuggable = true
            externalNativeBuild {
                cmake {
                    cppFlags += "-DBX_CONFIG_DEBUG=1"
                }
            }
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }

    // Configuración estable para el target de la JVM
    @Suppress("DEPRECATION")
    kotlinOptions {
        jvmTarget = "17"
    }

    sourceSets {
        getByName("main") {
            assets.directories.add("../../../core/assets")
        }
    }

    externalNativeBuild {
        cmake {
            path = file("../../../CMakeLists.txt")
        }
    }

    buildFeatures {
        prefab = true
    }
}

dependencies {
    implementation("androidx.games:games-frame-pacing:2.1.3")
    implementation("androidx.appcompat:appcompat:1.7.0")
}
