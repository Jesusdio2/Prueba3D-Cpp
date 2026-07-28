import { AbilityConstant, UIAbility, Want } from '@kit.AbilityKit';
import { window } from '@kit.ArkUI';
import { hilog } from '@kit.PerformanceAnalysisKit';

export default class EntryAbility extends UIAbility {
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    hilog.info(0x0000, 'GameActivity', 'Ability onCreate');
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    hilog.info(0x0000, 'GameActivity', 'Ability onWindowStageCreate');

    let windowClass: window.Window = windowStage.getMainWindowSync();

    // 1. Configurar pantalla completa inmersiva
    windowClass.setWindowLayoutFullScreen(true);

    // 2. Ocultar barras del sistema pasándole un arreglo vacío
    windowClass.setWindowSystemBarEnable([]);

    // 3. Forzar de forma directa que la app pinte detrás del Notch / Cutout
    windowClass.setWindowLayoutFullScreen(true, (err) => {
      if (!err.code) {
        hilog.info(0x0000, 'GameActivity', 'Succeeded in setting full screen cutouts.');
      }
    });

    // Cargar la vista principal de juego
    windowStage.loadContent('pages/Index', (err) => {
      if (err.code) {
        hilog.error(0x0000, 'GameActivity', 'Failed to load content. Cause: %{public}s', JSON.stringify(err) ?? '');
        return;
      }
      hilog.info(0x0000, 'GameActivity', 'Succeeded in loading content.');
    });
  }

}
