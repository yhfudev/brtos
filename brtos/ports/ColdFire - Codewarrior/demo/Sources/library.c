void* _MSL_CDECL __CALLOC(size_t nmemb, size_t size) 
{ 
  void* result; 
  __begin_critical_region(malloc_pool_access); 
  _calloc:
  calloc:
  asm{ 
    move.l   d7,-(a7)
    move.l   d6,-(a7)
    move.l   d0,d6
    move.l   d1,d7
  }
  result = __pool_alloc_clear(get_malloc_pool(), size*nmemb); 
  __end_critical_region(malloc_pool_access); 
  return result; 
  
  asm
  {  
    jsr      _get_malloc_pool
    muls.l   d7,d6
    move.l   d6,d0
    jsr      ___pool_alloc_clear
  }
} 

  move.l   (a7)+,d6
  move.l   (a7)+,d7
  rts
  
 
 
 
 
 
  
  
; 2416: void* _MSL_CDECL __pool_alloc_clear(__mem_pool* pool, size_t size) 
; 2417: { 
; 2418: 	void* result; 
; 2419: #ifdef _MSL_MALLOC_0_RETURNS_NON_NULL 
; 2420: 	if (size == 0) 
; 2421: 		size = 1; 
; 2422: #endif /* _MSL_MALLOC_0_RETURNS_NON_NULL */ 
;
0x00000000                    ___pool_alloc_clear:
;                             __pool_alloc_clear:
0x00000000  0x2F0E                   move.l   a6,-(a7)
0x00000002  0x2F07                   move.l   d7,-(a7)
0x00000004  0x2E00                   move.l   d0,d7
;
; 2423: 	result = __pool_allocate(pool, size, 0); 
;
0x00000006  0x4281                   clr.l    d1
0x00000008  0x2241                   movea.l  d1,a1
0x0000000A  0x4EBA0000               jsr      ___pool_allocate
0x0000000E  0x2C48                   movea.l  a0,a6
;
; 2424: 	if (result != 0) 
;
0x00000010  0x4A8E                   tst.l    a6
0x00000012  0x670A                   beq.s    *+12                  ; 0x0000001e
;
; 2425: 		memset(result, 0, size); 
;
0x00000014  0x2207                   move.l   d7,d1
0x00000016  0x4280                   clr.l    d0
0x00000018  0x4EB900000000           jsr      _memset
;
; 2426: 	return result; 
;
0x0000001E  0x204E                   movea.l  a6,a0
;
; 2427: } 
;
0x00000020  0x2E1F                   move.l   (a7)+,d7
0x00000022  0x2C5F                   movea.l  (a7)+,a6
0x00000024  0x4E75                   rts      
  