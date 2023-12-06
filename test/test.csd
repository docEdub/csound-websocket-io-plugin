<CsoundSynthesizer>
<CsOptions>
-n -d -M0 -+rtmidi=NULL --midi-key-cps=4 --midi-velocity-amp=5 -+msg_color=false
</CsOptions>
<CsInstruments>

nchnls = 1
sr = 48000
kr = 1
0dbfs = 1

instr 1
    SValue init ""
    kValue[] init 3

    SValue = websocket_getString(12345, "/test/1")
    kValue = websocket_getArray(12345, "/test/2")

    if (changed2(kValue) == 1) then
        printsk("websocket_get... SValue = %s: ", SValue)

        ki = 0
        printsk("kValue = [ ");
        while (ki < lenarray:k(kValue)) do
            if (ki > 0) then
                printsk(", ")
            endif
            printsk("%f", kValue[ki])
            ki += 1
        od
        printsk(" ]\n")
    endif
endin

instr 2
    SValue init "1, 2, 3"
    kValue[] = fillarray(1, 2, 3)

    websocket_set(12345, "/test/1", SValue)
    websocket_set(12345, "/test/2", kValue)
endin

</CsInstruments>
<CsScore>

i1 0 z
i2 0 z

</CsScore>
</CsoundSynthesizer>
