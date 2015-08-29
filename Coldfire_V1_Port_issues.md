

&lt;BR&gt;


**Para compilar o BRTOS no CodeWarrior versão 6.3 deve-se modificar a região ".custom" do arquivo "Project.lcf", ficando assim:**

```
  .custom :
  {
    ___HEAP_START       = .;
    ___heap_addr        = ___HEAP_START;
    ___HEAP_END         = ___HEAP_START + ___heap_size;
    ___SP_END             = ___HEAP_END;
    ___SP_INIT          = ___SP_END + ___stack_size;

    ___mem_limit        = ___HEAP_END;
    ___stack_safety     = 16;

    . = ALIGN (0x4);
  } >> userram

```



&lt;BR&gt;


**Para o port do Coldfire V1 sem suporte a "aninhamento de interrupções" é necessário configurar o registrador CPUCR da seguinte forma:**
```
  asm {
    // VBR: ADDRESS=0
    clr.l d0
    movec d0,VBR
    // CPUCR: ARD=0,IRD=0,IAE=0,IME=1,BWD=1,FSD=1
    move #0x12000000, d0
    movec d0,CPUCR
  }
```


&lt;BR&gt;


**Ainda, este microcontrolador apresenta um problema no módulo "Flash speculation" quando desabilitando o aninhamento de interrupções. Assim, recomenda-se que este módulo seja desabilitado neste modo de operação.**