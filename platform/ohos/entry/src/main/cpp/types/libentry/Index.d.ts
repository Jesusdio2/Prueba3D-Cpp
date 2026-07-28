export const add: (a: number, b: number) => number;
export const onKeyEvent: (keyCode: number, isPressed: boolean) => boolean;
export const initGameLoop: () => void;
export const shutdownGame: () => void;
export const setGameState: (state: number) => void;
export const onTouch: (x: number, y: number, action: number) => void;
export const shouldQuit: () => boolean;
