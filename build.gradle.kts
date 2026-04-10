// build.gradle.kts en la raíz del proyecto

plugins {
    // Plugin de Android y Kotlin se aplican en los módulos, no aquí
    // Aquí puedes aplicar plugins globales si los necesitas
}

buildscript {
    repositories {
        google()
        mavenCentral()
    }
    dependencies {
        classpath("com.android.tools.build:gradle:8.3.0")
        classpath(kotlin("gradle-plugin", version = "1.9.10")) // 🔹 actualizado a 1.9.10
    }
}

allprojects {
    repositories {
        google()
        mavenCentral()
    }
}