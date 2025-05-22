#include "keymap.h"
#include <cx/string.h>
#include <cx/utils.h>

static LazyInitState keymap_init_state;

hashtable keymap_iuptoftl;
hashtable keymap_iuptodisplay;
hashtable keymap_iuptostr;
hashtable keymap_strtoiup;

#define IUPKEY(m)                                        \
    htInsert(&keymap_iuptostr, int32, m, string, _S #m); \
    htInsert(&keymap_strtoiup, string, _S #m, int32, m)

#define DISPLAYSP(c, n)   \
    strCopy(&temp, _S n); \
    htInsert(&keymap_iuptodisplay, int32, c, string, temp)

#define IUPFTLMAP(iupkey, ftlkey) htInsert(&keymap_iuptoftl, int32, iupkey, int32, ftlkey)

static void keymap_init(void* unused)
{
    htInit(&keymap_iuptoftl, int32, int32, 256);
    htInit(&keymap_iuptostr, int32, string, 256);
    htInit(&keymap_iuptodisplay, int32, string, 256);
    htInit(&keymap_strtoiup, string, int32, 256);

    IUPKEY(K_SP);
    IUPKEY(K_exclam);
    IUPKEY(K_quotedbl);
    IUPKEY(K_numbersign);
    IUPKEY(K_dollar);
    IUPKEY(K_percent);
    IUPKEY(K_ampersand);
    IUPKEY(K_apostrophe);
    IUPKEY(K_parentleft);
    IUPKEY(K_parentright);
    IUPKEY(K_asterisk);
    IUPKEY(K_plus);
    IUPKEY(K_comma);
    IUPKEY(K_minus);
    IUPKEY(K_period);
    IUPKEY(K_slash);
    IUPKEY(K_0);
    IUPKEY(K_1);
    IUPKEY(K_2);
    IUPKEY(K_3);
    IUPKEY(K_4);
    IUPKEY(K_5);
    IUPKEY(K_6);
    IUPKEY(K_7);
    IUPKEY(K_8);
    IUPKEY(K_9);
    IUPKEY(K_colon);
    IUPKEY(K_semicolon);
    IUPKEY(K_less);
    IUPKEY(K_equal);
    IUPKEY(K_greater);
    IUPKEY(K_question);
    IUPKEY(K_at);
    IUPKEY(K_A);
    IUPKEY(K_B);
    IUPKEY(K_C);
    IUPKEY(K_D);
    IUPKEY(K_E);
    IUPKEY(K_F);
    IUPKEY(K_G);
    IUPKEY(K_H);
    IUPKEY(K_I);
    IUPKEY(K_J);
    IUPKEY(K_K);
    IUPKEY(K_L);
    IUPKEY(K_M);
    IUPKEY(K_N);
    IUPKEY(K_O);
    IUPKEY(K_P);
    IUPKEY(K_Q);
    IUPKEY(K_R);
    IUPKEY(K_S);
    IUPKEY(K_T);
    IUPKEY(K_U);
    IUPKEY(K_V);
    IUPKEY(K_W);
    IUPKEY(K_X);
    IUPKEY(K_Y);
    IUPKEY(K_Z);
    IUPKEY(K_bracketleft);
    IUPKEY(K_backslash);
    IUPKEY(K_bracketright);
    IUPKEY(K_circum);
    IUPKEY(K_underscore);
    IUPKEY(K_grave);
    IUPKEY(K_a);
    IUPKEY(K_b);
    IUPKEY(K_c);
    IUPKEY(K_d);
    IUPKEY(K_e);
    IUPKEY(K_f);
    IUPKEY(K_g);
    IUPKEY(K_h);
    IUPKEY(K_i);
    IUPKEY(K_j);
    IUPKEY(K_k);
    IUPKEY(K_l);
    IUPKEY(K_m);
    IUPKEY(K_n);
    IUPKEY(K_o);
    IUPKEY(K_p);
    IUPKEY(K_q);
    IUPKEY(K_r);
    IUPKEY(K_s);
    IUPKEY(K_t);
    IUPKEY(K_u);
    IUPKEY(K_v);
    IUPKEY(K_w);
    IUPKEY(K_x);
    IUPKEY(K_y);
    IUPKEY(K_z);
    IUPKEY(K_braceleft);
    IUPKEY(K_bar);
    IUPKEY(K_braceright);
    IUPKEY(K_tilde);

    IUPKEY(K_BS);
    IUPKEY(K_TAB);
    IUPKEY(K_LF);
    IUPKEY(K_CR);

    IUPKEY(K_PAUSE);
    IUPKEY(K_ESC);
    IUPKEY(K_HOME);
    IUPKEY(K_LEFT);
    IUPKEY(K_UP);
    IUPKEY(K_RIGHT);
    IUPKEY(K_DOWN);
    IUPKEY(K_PGUP);
    IUPKEY(K_PGDN);
    IUPKEY(K_END);
    IUPKEY(K_MIDDLE);
    IUPKEY(K_Print);
    IUPKEY(K_INS);
    IUPKEY(K_Menu);
    IUPKEY(K_DEL);
    IUPKEY(K_F1);
    IUPKEY(K_F2);
    IUPKEY(K_F3);
    IUPKEY(K_F4);
    IUPKEY(K_F5);
    IUPKEY(K_F6);
    IUPKEY(K_F7);
    IUPKEY(K_F8);
    IUPKEY(K_F9);
    IUPKEY(K_F10);
    IUPKEY(K_F11);
    IUPKEY(K_F12);
    IUPKEY(K_F13);
    IUPKEY(K_F14);
    IUPKEY(K_F15);
    IUPKEY(K_F16);
    IUPKEY(K_F17);
    IUPKEY(K_F18);
    IUPKEY(K_F19);
    IUPKEY(K_F20);

    IUPKEY(K_LSHIFT);
    IUPKEY(K_RSHIFT);
    IUPKEY(K_LCTRL);
    IUPKEY(K_RCTRL);
    IUPKEY(K_LALT);
    IUPKEY(K_RALT);

    IUPKEY(K_NUM);
    IUPKEY(K_SCROLL);
    IUPKEY(K_CAPS);
    IUPKEY(K_CLEAR);
    IUPKEY(K_HELP);

    IUPKEY(K_NP_MULTIPLY);
    IUPKEY(K_NP_DIVIDE);
    IUPKEY(K_NP_ADD);
    IUPKEY(K_NP_SUBTRACT);
    IUPKEY(K_NP_DECIMAL);

    string temp = 0;
    for (int i = K_exclam; i <= K_tilde; i++) {
        char* buf = strBuffer(&temp, 1);
        if (i >= 'a' && i <= 'z')   // always show these in caps
            buf[0] = i - 'a' + 'A';
        else
            buf[0] = i;
        strSetLen(&temp, 1);

        // exception!
        if (i == K_grave || i == K_tilde)
            strCopy(&temp, _S"` (~)");
        htInsert(&keymap_iuptodisplay, int32, i, string, temp);
    }

    // some friendlier names for a few keys
    // TODO: translate?
    DISPLAYSP(K_SP, "Space");
    DISPLAYSP(K_BS, "BkSp");
    DISPLAYSP(K_TAB, "Tab");
    DISPLAYSP(K_CR, "Enter");
    DISPLAYSP(K_PAUSE, "Pause");
    DISPLAYSP(K_ESC, "Escape");   // can't actually be bound, omit?
    DISPLAYSP(K_HOME, "Home");
    DISPLAYSP(K_LEFT, "Left");
    DISPLAYSP(K_UP, "Up");
    DISPLAYSP(K_RIGHT, "Right");
    DISPLAYSP(K_DOWN, "Down");
    DISPLAYSP(K_PGUP, "PgUp");
    DISPLAYSP(K_PGDN, "PgDown");
    DISPLAYSP(K_END, "End");
    DISPLAYSP(K_INS, "Insert");
    DISPLAYSP(K_DEL, "Delete");
    DISPLAYSP(K_F1, "F1");
    DISPLAYSP(K_F2, "F2");
    DISPLAYSP(K_F3, "F3");
    DISPLAYSP(K_F4, "F4");
    DISPLAYSP(K_F5, "F5");
    DISPLAYSP(K_F6, "F6");
    DISPLAYSP(K_F7, "F7");
    DISPLAYSP(K_F8, "F8");
    DISPLAYSP(K_F9, "F9");
    DISPLAYSP(K_F10, "F10");
    DISPLAYSP(K_F11, "F11");
    DISPLAYSP(K_F12, "F12");
    DISPLAYSP(K_F13, "F13");
    DISPLAYSP(K_F14, "F14");
    DISPLAYSP(K_F15, "F15");
    DISPLAYSP(K_F16, "F16");
    DISPLAYSP(K_F17, "F17");
    DISPLAYSP(K_F18, "F18");
    DISPLAYSP(K_F19, "F19");
    DISPLAYSP(K_F20, "F20");

    DISPLAYSP(K_NP_MULTIPLY, "NP *");
    DISPLAYSP(K_NP_DIVIDE, "NP /");
    DISPLAYSP(K_NP_ADD, "NP +");
    DISPLAYSP(K_NP_SUBTRACT, "NP -");
    DISPLAYSP(K_NP_DECIMAL, "NP .");
    strDestroy(&temp);

    // build IUP to FTL table
    IUPFTLMAP(K_BS, K_BS);
    IUPFTLMAP(K_TAB, K_TAB);
    IUPFTLMAP(K_CR, K_CR);
    IUPFTLMAP(K_PAUSE, 19);
    IUPFTLMAP(K_ESC, 27);

    // these are all the same
    for (int i = K_SP; i <= K_at; i++) {
        IUPFTLMAP(i, i);
    }

    // FTL always uses the lowercase versions, so map to them
    for (int i = K_A; i <= K_Z; i++) {
        IUPFTLMAP(i, i - K_A + K_a);
    }

    // these are all the same
    for (int i = K_bracketleft; i <= K_tilde; i++) {
        IUPFTLMAP(i, i);
    }

    IUPFTLMAP(K_DEL, 127);
    IUPFTLMAP(K_NP_DECIMAL, 266);
    IUPFTLMAP(K_NP_DIVIDE, 267);
    IUPFTLMAP(K_NP_MULTIPLY, 268);
    IUPFTLMAP(K_NP_SUBTRACT, 269);
    IUPFTLMAP(K_NP_ADD, 270);
    IUPFTLMAP(K_UP, 273);
    IUPFTLMAP(K_DOWN, 274);
    IUPFTLMAP(K_RIGHT, 275);
    IUPFTLMAP(K_LEFT, 276);
    IUPFTLMAP(K_INS, 277);
    IUPFTLMAP(K_HOME, 278);
    IUPFTLMAP(K_END, 279);
    IUPFTLMAP(K_PGUP, 280);
    IUPFTLMAP(K_PGDN, 281);
    for (int i = K_F1; i <= K_F15; i++) {
        IUPFTLMAP(i, i - K_F1 + 282);
    }
    IUPFTLMAP(K_NUM, 300);
    IUPFTLMAP(K_CAPS, 301);
    IUPFTLMAP(K_LSHIFT, 304);
    IUPFTLMAP(K_RSHIFT, 304);
    IUPFTLMAP(K_RCTRL, 305);
    IUPFTLMAP(K_LCTRL, 306);
    IUPFTLMAP(K_RALT, 307);
    IUPFTLMAP(K_LALT, 308);
}

void keymapInit()
{
    lazyInit(&keymap_init_state, keymap_init, NULL);
}
