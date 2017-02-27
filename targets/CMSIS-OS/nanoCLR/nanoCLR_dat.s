@
@ Copyright (c) 2017 The nanoFramework project contributors
@ See LICENSE file in the project root for full license information.
@

     .align 4
     .section   nanoclr_metadata, "a", %progbits

    .global  nanoCLR_Dat_Start
    .global  nanoCLR_Dat_End

nanoCLR_Dat_Start:
    @ TODO FIXME
    @.incbin "nanoclr.dat"
nanoCLR_Dat_End:

    .end
