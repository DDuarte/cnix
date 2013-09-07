Especificação de Projeto
===================

Turma 1 - Grupo 5
-----------------

*Duarte Duarte* - ei11101

*Miguel Marques* - ei11099


**Editor de Código**


##### Descrição:

Programa que permite editar (abrir -> alterar -> guardar) vários ficheiros de texto ou código (em C) em simultâneo (usando separadores).

Nos ficheiros de C as palavras-chave da linguagem serão destacadas (*syntax highlighting*).

Será também possível compilar o código e mostrar resultado da compilação na interface do editor.


##### Dispositivos a utilizar:

- *RTC* (detalhes sobre a data/tempo de edição de um ficheiro)
    - Com interrupções
    - *Handler* em Assembly

- *Timer* (temporizador para interrupçoes)
    - Com interrupções
    - *Handler* em Assembly

- Placa gráfica - modo gráfico (*interface* com o utilizador)
    - *Double-buffering*
    - "Limpar" e trocar o *buffer* da placa gráfica em Assembly

- Rato (interação com a interface gráfica, posicionar cursor, etc.)
    - *Polling*

- Teclado (*input* do utilizador, teclas de atalho, etc.)
    - Com interrupções

- *Speaker* (aviso em caso de erros de compilação)


##### Funcionalidades:

- Edição de texto (múltiplos ficheiros ("tabs"))

- Abrir ficheiros

- Guardar ficheiros

- Compilar código (em C)

- Mostrar erros de compilação

- *Syntax highlighting* (básico)


##### Módulos:

- Interrupções (interrupt.h):
    - Subscrição e cancelamento de interrupções
    - Gestão de interrupções em tempo de execução com o auxílio de *callbacks*.

```
    void int_init(void);
    void int_init(void);
    int  int_subscribe(int irq_line, int policy, void (*callback)());
    int  int_unsubscribe(int bit);
    int  int_start_handler(void);
    void int_stop_handler(void);
```

- Módulo gráfico (vbe.h, video_gr.h)
    - Inicialização do módulo gráfico
    - Preencher ecrã
    - Desenhar pontos, linhas, rectângulos, etc.
    - Desenhar carácteres
    - Desenhar bitmaps
    - Trocar o *buffer* (*double-buffering*)

```
    void* vg_init(unsigned short mode);
    int vg_swap_buffer(void);
    int vg_exit(void);
    int vg_fill(unsigned long color);
    int vg_set_pixel(unsigned long x, unsigned long y, unsigned long color);
    long vg_get_pixel(unsigned long x, unsigned long y);
    int vg_draw_line(unsigned long xi, unsigned long yi, unsigned long xf,
        unsigned long yf, unsigned long color);
    int vg_draw_rectangle(unsigned long x1, unsigned long y1,
        unsigned long x2, unsigned long y2, unsigned long color);
    int vg_draw_circle(int x0, int y0, int radius, unsigned long color);
    int vg_draw_character(char c, double scale, unsigned long x,
        unsigned long y, unsigned long color);
    int vg_draw_string(char* str, double scale, unsigned long x,
        unsigned long y, unsigned long color);
    int vg_draw_bitmap(bitmap* bm, double scale, double rotation,
        unsigned long x, unsigned long y);

    int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p);
```

- Módulo input/output (kbd_mouse.h)
    - Gerir estado do teclado e rato

```
    int mouse_init(void);
    int kbd_init(void);
    mouse_state_t get_mouse_state(void);
    kbd_state_t get_kbd_state(void);
    int kbd_is_key_down(KBD_KEY key);
    int kbd_is_key_up(KDB_key key);
```

- Módulo speaker (speaker.h)
    - Emitir som

```
    void speaker_beep(unsigned int freq, unsigned int dur_ms);
```

- Módulo RTC (rtc.h)
    - Obter hora do sistema
    - Temporizador de alta resolução

```
    int rtc_get_date_str(char* dst);
    int rtc_get_date(date_t* dst);
    int rtc_sleep(unsigned int dur_ms); /* modo polling */
    int rtc_set_callback(void (*cb)(), unsigned int dur_ms); /* modo interrupt */
```

- Módulo timer (timer.h)
    - Temporizador de baixa resolução (60 Hz; para polling, etc.)

```
    int timer_sleep(unsigned int dur_s);
    int timer_set_callback(void (*cb)(), unsigned int dur_s);
```

- Módulo menu (menu.h)
    - Menu com sub-menus

- Módulo window (window.h)
    - Representa a janela principal do programa

- Módulo tab (tab.h)
    - Caixa de texto para cada ficheiro a editar

- Módulo utilidades
    - Estruturas de dados comuns (queue, stack, vector, list, etc.)
    - Funções e definições auxiliares


##### Planeamento:

- 1ª semana:
    - Desenho da janela principal
    - Funções *draw* finalizadas (includindo draw_character)

- 2ª semana:
    - Configuração de timer(s) e RTC
    - Rato e teclado
    - Implementar menu

- 3ª semana:
    - Implementar tabs (separadores)
    - Leitura e escrita de ficheiros
    - Permitir executar comandos para compilação do código bem como visualização do resultado

- 4ª semana:
    - *Syntax-highlighting*
    - Finalizar a interface gráfica
    - < Reservado para possíveis atrasos >


##### Objetivos opcionais:

- Possibilidade de minimizar a janela voltando à consola do Minix

- Melhorar *syntax-highlighting* recorrendo a um analisador de código

- Transferir ficheiros entre computadores recorrendo à *serial port*

---------------

*Laboratórios de Computadores 2012/2013* - *MIEIC*
