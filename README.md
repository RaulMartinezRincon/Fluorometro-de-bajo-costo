# Fluorometro-de-bajo-costo
Este dispositivo está pensado para medir la fluorescencia en muestras de agua con microalgas. El sensor de luz (AS7341) mide 8 longitudes de onda en la parte visible de la luz. La fluorescencia se detectó en la banda F8 (670 - 690 nm). Se utilizó un láser centrado en longitud de onda de 405 nm para estimular la fluorescencia.

- DFRobot Gravity AS7341 de alta precisión, Sensor espectral de luz visible de 11 canales y 6 canales ADC de 16 bits.
- Longitud de onda: 405nm, Potencia de salida: 20mW, Voltaje de funcionamiento: 3.0-3.6 V, Temperatura de trabajo: -10 ℃ ~ + 40 ℃, Corriente de funcionamiento: I < 100mA, Dimensiones: 12*35mm 

Cosas por probar:
- Contar célutas al mismo tiempo que se hace la lectura de fluorescencia para tratar de estimar densidad celular a partir de la fluorescencia.
- Usar un lente para aumentar los valores de fluorescencia. Esto permitirá detectar valores bajos de clorofila-a.
- Usar un láser más potente.
- Usar un PWM para cambiar la potencia del láser y detectar una saturación de la fluorescencia.
