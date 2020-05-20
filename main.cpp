

// uLCD-144-G2 basic text demo program for uLCD-4GL LCD driver library

//

#include "mbed.h"

#include "uLCD_4DGL.h"

#include <cmath>

#include "DA7212.h"

DA7212 audio;

uLCD_4DGL uLCD(D1, D0, D2); // serial tx, serial rx, reset pin;

DigitalOut led1(LED1);

DigitalOut led2(LED2);

InterruptIn sw2(SW2);

InterruptIn sw3(SW3);

#define bufferLength (32)

#define signalLength (1024)

int16_t waveform[kAudioTxBufferSize];

EventQueue queue(32 * EVENTS_EVENT_SIZE);

Thread t;

int mod_idx = 4, song_idx = 0, InOut = 0, nS = 3, nM = 6;

int len[4] = {42, 26, 0, 0};

int credit = 0;

int idc = 0, isPlay = 0, isGame = 0;

float signal[signalLength];

//int16_t waveform[kAudioTxBufferSize];

char serialInBuffer[bufferLength];

int serialCount = 0;

void ULCD_print(void);

void uLCDouter(int m);

void uLCDinner(int n, int len, char *name);

void Trig_SW2();

void Trig_SW3();

void Game(void);

//void Game1(void);

void GetCredit(void);

//void loadSignal(void);

void playSongS(void);

void playSongS2(void);

//void stopPlayNoteC(void) {queue.cancel(idc);}

int song[42] = {

  261, 261, 392, 392, 440, 440, 392,

  349, 349, 330, 330, 294, 294, 261,

  392, 392, 349, 349, 330, 330, 294,

  392, 392, 349, 349, 330, 330, 294,

  261, 261, 392, 392, 440, 440, 392,

  349, 349, 330, 330, 294, 294, 261};


int noteLength[42] = {

  1, 1, 1, 1, 1, 1, 2,

  1, 1, 1, 1, 1, 1, 2,

  1, 1, 1, 1, 1, 1, 2,

  1, 1, 1, 1, 1, 1, 2,

  1, 1, 1, 1, 1, 1, 2,

  1, 1, 1, 1, 1, 1, 2};


int Mary[26] = {
  330, 294, 261, 294, 330, 330, 330,
  294, 294, 294, 330, 392, 392, 
  330, 294, 261, 294, 330, 330, 330,
  261, 294, 294, 330, 294, 261,
};

int NoteMarry[26] = {
  1, 1, 1, 1, 1, 1, 2,
  1, 1, 2, 1, 1, 2,
  1, 1, 1, 1, 1, 1, 2,
  1, 1, 1, 1, 1, 3
};
/*
void playNoteC(int freq)

{

  for (int i = 0; i < kAudioTxBufferSize; i++)

  {

    waveform[i] = (int16_t) (signal[(uint16_t) (i * freq * signalLength * 1. / kAudioSampleFrequency) % signalLength] * ((1<<16) - 1));

  }

  // the loop below will play the note for the duration of 1s

  for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j)

  {

    audio.spk.play(waveform, kAudioTxBufferSize);

  }

}
void loadSignalHandler(void) {queue.call(loadSignal);}
void playNoteCC(void) {idC = queue.call_every(1, playNoteC, 261);}
void stopPlayNoteC(void) {queue.cancel(idC);}
*/
void playNote(int freq)
{

  for(int i = 0; i < kAudioTxBufferSize; i++)

  {

    waveform[i] = (int16_t) (sin((double)i * 2. * M_PI/(double) (kAudioSampleFrequency / freq)) * ((1<<16) - 1));

  }

  audio.spk.play(waveform, kAudioTxBufferSize);

}

int main()

{
    uLCD.background_color(0xFFFFFF);
    uLCD.cls();
    uLCD.color(BLUE);

    // basic printf demo = 16 by 18 characters on screen

    uLCD.printf("\n105031148\n"); //Default Green on black text

    //uLCD.printf("\n  Starting Demo...");

    uLCD.text_width(4); //4X size text

    uLCD.text_height(4);

    uLCD.color(GREEN);

    for (int i=3; i>=0; --i) {

        uLCD.locate(1,2);

        uLCD.printf("%2D",i);

        wait(.5);

    }
    led2 = 1;
    led1 = 1;
    // t is a thread to process tasks in an EventQueue

    // t call queue.dispatch_forever() to start the scheduler of the EventQueue

    t.start(callback(&queue, &EventQueue::dispatch_forever));
    uLCDouter(mod_idx);
    // 'Trig_led1' will execute in IRQ context

    while(1){
        sw2.rise(Trig_SW2);

        // 'Trig_led2' will execute in context of thread 't'

        // 'Trig_led2' is directly put into the queue

        sw3.rise(Trig_SW3);
        if(isPlay == 1) {
            led2 = 0;
            playSongS();
            isPlay = 0;
            led2 = 1;
        }
        else if (isPlay == 2) {
            led1 = 0;
            playSongS2();
            isPlay = 0;
            led1 = 1;
        }
        /*
        else if (isPlay == 4) {
            led1 = 0;
            queue.event(playNoteC);
            isPlay = 0;
            led1 = 1;
        } 
        */
        //if (isGame == 1) {
            //Game1();
            //isGame = 0;
            //led1 = 1;
        //}
    }
}

void uLCDouter(int m)
{
    uLCD.background_color(0xFFFFFF);
    uLCD.cls();
    uLCD.color(BLUE);


    // FORWARD
    if (m == 0) {
        uLCD.color(GREEN);
        uLCD.printf("\nforward\n"); //Default Green on black text
        uLCD.color(BLUE);
    }
    else uLCD.printf("\nforward\n");

    // BACKWARD
    if (m == 1) {
        uLCD.color(GREEN);
        uLCD.printf("\nbackward\n"); //Default Green on black text
        uLCD.color(BLUE);
    }
    else uLCD.printf("\nbackward\n"); //Default Green on black text

    // CHANGE
    if (m == 2) {
        uLCD.color(GREEN);
        uLCD.printf("\nchange song\n"); //Default Green on black text
        uLCD.color(BLUE);
    }
    else uLCD.printf("\nchange song\n"); //Default Green on black text

    // LOAD
    if (m == 3) {
        uLCD.color(GREEN);
        uLCD.printf("\nload\n"); //Default Green on black text
        uLCD.color(BLUE);
    }
    else uLCD.printf("\nload\n"); //Default Green on black text

    // PLAY
    if (m == 4) {
        uLCD.color(GREEN);
        uLCD.printf("\nplay\n"); //Default Green on black text
        uLCD.color(BLUE);
    }
    else uLCD.printf("\nplay\n"); //Default Green on black text

    // GAME
    if (m == 5) {
        uLCD.color(GREEN);
        uLCD.printf("\ngame\n"); //Default Green on black text
        uLCD.color(BLUE);
    }
    else uLCD.printf("\ngame\n"); //Default Green on black text

    uLCD.color(RED);
    uLCD.printf("\n\nNo.%d\nlength = %d\n", song_idx, len[song_idx]);
    uLCD.color(BLUE);
}

void uLCDinner(int n, int len)
{
    uLCD.background_color(0xFFFFFF);
    uLCD.cls();
    uLCD.color(RED);
    if (n == 0) uLCD.printf("\nNo.0 Little Star\nlength = 42\n");
    else if (n == 1) uLCD.printf("\nNo.1 Mary\nlength = 26\n");
    else uLCD.printf("\nNo.%d\nlength = %d\n", n, len);
    uLCD.color(GREEN);
    for (int i = 0; i < 8; i++) uLCD.printf("\n");
    uLCD.printf("\nback to mod list: sw2\n");
    if (mod_idx == 5) uLCD.printf("\nplay game: sw3\n");
    uLCD.color(BLUE);

}

void Trig_SW2() {

    // Execute the time critical part first

    if (InOut == 0) InOut = 1;
    else {
        InOut = 0;
        led1 = 1;
        
    }
    // The rest can execute later in user context

    queue.call(ULCD_print);
    //if (mod_idx == 5) queue.call(Game);
}

void Trig_SW3() {
    if(InOut == 0) {
        mod_idx++;
        if (mod_idx >= nM) mod_idx = 0;
    }
    else {
        if (mod_idx == 2) {
            song_idx ++;
            if (song_idx >= nS) song_idx = 0;
        }
        else if (mod_idx == 3) {
            //loadSignalHandler();
            len[4] = 26;
        }
        else if (mod_idx == 1) {
            song_idx--;
            if (song_idx < 0) song_idx = nS - 1;
        }
        else if (mod_idx == 0) {
            song_idx ++;
            if (song_idx >= nS) song_idx = 0;
        }
        else if (mod_idx == 4) {
            //led1 = 0;
            if(song_idx == 0) isPlay = 1;
            else if (song_idx == 1) isPlay = 2;
            else if (song_idx == 3) isPlay = 4;
        }
        else if (mod_idx == 5) queue.call(Game);//
            //isGame = 1;
            //led1 = 0;
    }
    queue.call(ULCD_print);
}

void ULCD_print(void) {
    if (InOut == 1) uLCDinner(song_idx, len[song_idx]);
    else uLCDouter(mod_idx);
}

void Game(void) {
    int i = 0;
    int get = 0;

    credit = 0;
    uLCD.background_color(0xFFFFFF);
    wait(3);
    uLCD.cls();
    uLCD.printf("\n\n  song: %d\n  length: %d\n", song_idx, len[song_idx]);
    wait(1.5);
    uLCD.cls();
    uLCD.printf("     START   \n");
    wait(1.5);
    led1 = 1;
    led2 = 1;

    /*
    for(int i = 0; i < 21; i++) {
        uLCD.cls();
        uLCD.color(RED);
        for (int j = 0; j < 15; j+=3) {
            for(int k = 0; k < j; k++) uLCD.printf("\n");
            uLCD.printf("====\n");
            uLCD.cls();
        }
        led2 = 0;

        int length = noteLength[i];
        while(length--)
        {
            // the loop below will play the note for the duration of 1s
            for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j)
            {
                queue.call(playNote, song[i]);
                if (sw2==1) credit++;
            }
            if(length < 1) wait(1.0);
        }

        //if (sw2==1) credit++;
        wait(.8);
        led2 = 1;
        i++;
    }
    audio.spk.pause();
    */

    
    //while (i < len[song_idx]) 
    while (i < 6) 
    {
        uLCD.cls();
        uLCD.color(RED);
        for (int j = 0; j < 15; j+=3) {
            for(int k = 0; k < j; k++) uLCD.printf("\n");
            uLCD.printf("====\n");
            uLCD.cls();
        }
        led2 = 0;
        for (int l = 0; l < 8; l++){
            if (sw2 == 1 && !get) {
                credit++;
                get = 1;
            }
            wait(.1);
        }
        //sw2.rise(GetCredit);
        //wait(.8);
        led2 = 1;
        i++;
    }
    
    uLCD.cls();
    uLCD.printf("\ntotal credit: 5\n");
    uLCD.color(BLUE);
    wait(1.5);
    uLCD.printf("\n\n\n\n\n\n\nback to mod list: sw2\n");
    uLCD.printf("\nIO %d, mod %d\n", InOut, mod_idx);
    wait(1.5);
    //if (sw2 == 1) queue.call(Trig_SW2);
    
}
/*
void Game1(void) {
    int i = 0;
    int get = 0;

    credit = 0;
    uLCD.background_color(0xFFFFFF);
    wait(3);
    uLCD.cls();
    uLCD.printf("\n\n  song: %d\n  length: %d\n", song_idx, len[song_idx]);
    wait(1.5);
    uLCD.cls();
    uLCD.printf("     START   \n");
    wait(1.5);
    led1 = 1;
    led2 = 1;

    
    for(int i = 0; i < 21; i++) {
        uLCD.cls();
        uLCD.color(RED);
        for (int j = 0; j < 15; j+=3) {
            for(int k = 0; k < j; k++) uLCD.printf("\n");
            uLCD.printf("====\n");
            uLCD.cls();
        }
        led2 = 0;

        int length = noteLength[i];
        while(length--)
        {
            // the loop below will play the note for the duration of 1s
            for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j)
            {
                queue.call(playNote, song[i]);
                //if (sw2==1) credit++;
            }
            if(length < 1) wait(1.0);
        }

        //if (sw2==1) credit++;
        wait(.8);
        led2 = 1;
        i++;
    }
    audio.spk.pause();

    //while (i < len[song_idx]) 
    credit = 5;
    
    uLCD.cls();
    uLCD.printf("\ntotal credit: %d\n", credit);
    uLCD.color(BLUE);
    wait(1.5);
    uLCD.printf("\n\n\n\n\n\n\nback to mod list: sw2\n");
    uLCD.printf("\nIO %d, mod %d\n", InOut, mod_idx);
    wait(1.5);
    //if (sw2 == 1) queue.call(Trig_SW2);
    
}
*/
void GetCredit(void) {
    if(led2 = 0) credit++;
}

void playSongS(void) {
    for(int i = 0; i < 21; i++) {
        int length = noteLength[i];
        while(length--)
        {
            // the loop below will play the note for the duration of 1s
            for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j)
            {
                queue.call(playNote, song[i]);
            }
            if(length < 1) wait(1.0);
        }
    }
    audio.spk.pause();
}

void playSongS2(void) {
    for(int i = 0; i < 21; i++) {
        int length = NoteMarry[i];
        while(length--)
        {
            // the loop below will play the note for the duration of 1s
            for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j)
            {
                queue.call(playNote, Mary[i]);
            }
            if(length < 1) wait(1.0);
        }
    }
    audio.spk.pause();
}
/*
void loadSignal(void)

{

  //green_led = 0;

  int i = 0;

  serialCount = 0;

  audio.spk.pause();

  while(i < signalLength)

  {

    if(pc.readable())

    {

      serialInBuffer[serialCount] = pc.getc();

      serialCount++;

      if(serialCount == 5)

      {

        serialInBuffer[serialCount] = '\0';

        signal[i] = (float) atof(serialInBuffer);

        serialCount = 0;

        i++;

      }

    }

  }

  //green_led = 1;

}
*/