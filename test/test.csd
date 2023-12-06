<CsoundSynthesizer>
<CsOptions>
-n -d -M0 -+rtmidi=NULL --midi-key-cps=4 --midi-velocity-amp=5 -+msg_color=false
</CsOptions>
<CsInstruments>

nchnls = 1
sr = 48000
kr = 1
0dbfs = 1

opcode intFromString_k, k, S
    Svalue xin

    kgoto skip_i_time
    Svalue init "0"
    skip_i_time:

    kValue = strtolk(Svalue)
    xout kValue
endop

instr 1
    SValue1 init ""
    kValue2[] init 3

    SValue1 = websocket_getString(12345, "/test/1")
    kValue2 = websocket_getArray(12345, "/test/2")
    if (changed2(kValue2) == 1) then
        printsk("websocket_get... SValue1 = %s: ", SValue1)

        ki = 0
        printsk("kValue = [ ");
        while (ki < lenarray:k(kValue2)) do
            if (ki > 0) then
                printsk(", ")
            endif
            printsk("%f", kValue2[ki])
            ki += 1
        od
        printsk(" ]\n")
    endif
endin

instr 2
    SValue1 init "1, 2, 3"
    kValue2[] = fillarray(1, 2, 3)

    ; websocket_set(54321, "/test/1", SValue1)
    ; websocket_set(54321, "/test/2", kValue2)
    websocket_set(12345, "/test/1", SValue1)
    websocket_set(12345, "/test/2", kValue2)
endin

</CsInstruments>
<CsScore>

i1 0 z
i2 0 z

</CsScore>
</CsoundSynthesizer>
