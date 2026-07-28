import { promptAction, window } from '@kit.ArkUI';
import { common } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import nativeEngine from 'libentry.so';

@Entry
@Component
struct Index {
  @State isEngineReady: boolean = false;
  private context = getContext(this) as common.UIAbilityContext;
  private timerId: number = -1;

  aboutToAppear() {
    if (nativeEngine && typeof nativeEngine.onKeyEvent === 'function') {
      this.isEngineReady = true;
    } else {
      this.mostrarErrorCritico();
    }
  }

  aboutToDisappear() {
    // Detener el chequeo de salida e informar al motor
    if (this.timerId !== -1) {
      clearInterval(this.timerId);
    }
    nativeEngine.shutdownGame();
  }

  mostrarErrorCritico() {
    promptAction.showDialog({
      title: 'Error Crítico',
      message: 'No se pudieron cargar los componentes gráficos de C++. La aplicación se cerrará.',
      buttons: [{ text: 'Aceptar', color: '#FF0000' }]
    }).then(() => {
      this.context.terminateSelf();
    });
  }

  build() {
    Row() {
      Column() {
        if (this.isEngineReady) {
          XComponent({
            id: 'bgfxSurface',
            type: XComponentType.SURFACE,
            libraryname: 'entry' // Liga el ciclo de vida del Surface a napi_init.cpp
          })
            .width('100%')
            .height('100%')
            .backgroundColor(Color.Black)

            // 1. Inicialización del bucle cuando la superficie está lista
            .onLoad(() => {
              // Arranca el loop en C++
              nativeEngine.initGameLoop();

              // Monitorear de manera asíncrona si el core de C++ solicita salir (shouldQuit)
              this.timerId = setInterval(() => {
                if (nativeEngine.shouldQuit()) {
                  this.context.terminateSelf();
                }
              }, 100);
            })

            // 2. Control de eventos táctiles (Equivalente a onTouchEvent de Android)
            .onTouch((event: TouchEvent) => {
              if (!event || event.touches.length === 0) return;
              let touch = event.touches[0];

              // Mapeo del estado mecánico: 0=Down, 1=Move, 2=Up (Ajusta según tu core)
              let nativeAction = 0;
              if (event.type === TouchType.Move) nativeAction = 1;
              if (event.type === TouchType.Up) nativeAction = 2;

              nativeEngine.onTouch(touch.x, touch.y, nativeAction);
            })

            // 3. Controladores de Teclado (Equivalente a onKeyDown y onKeyUp)
            .onKeyEvent((event: KeyEvent) => {
              if (!event) return;
              let isPressed = (event.type === 1);
              let consumed = nativeEngine.onKeyEvent(event.keyCode, isPressed);
              if (consumed) {
                event.stopPropagation();
              }
            })
        }
      }
      .width('100%')
    }
    .height('100%')
  }
}
