;------------------------------------------------------------------------------
;
; Vertex Shader file for pgTerrainPatchMorphHW
; This Shader does the patches' geo-morphing
;
; $Id: TerrainMorph.vsh,v 1.5 2003/03/09 19:12:20 daniel Exp $
;
;------------------------------------------------------------------------------
;
; Constants specified by the app
;
;    c0      = (factorSelf, 0.0f, 0.5f, 1.0f)
;    c2      = (factorLeft, factorLeft2, factorRight, factorRight2),
;    c3      = (factorBottom, factorBottom2, factorTop, factorTop2)
;
;    c4-c7   = WorldViewProjection Matrix
;    c8-c11  = Pass 0 Texture Matrix
;
;
; Vertex components (as specified in the vertex DECLARATION)
;
;    v0      = (posX, posZ, texX, texY)
;    v1      = (posY, yMoveSelf, 0.0, 1.0)
;    v2      = (yMoveLeft, yMoveLeft2, yMoveRight, yMoveRight2)
;    v3      = (yMoveBottom, yMoveBottom2, yMoveTop, yMoveTop2)
;
;
;------------------------------------------------------------------------------

; We use Vertex Shader 1.1
;
vs.1.1


;------------------------------------------------------------------------------
; Vertex transformation
;------------------------------------------------------------------------------

mov r0, v0.xzyy                    ; build the base vertex
mov r0.w, c0.w                     ; set w-component to 1.0

;mov r0.y, v1.x                    ; alternativly: simply use the lowres vertex

dp4 r1.x, v2, c2                   ; calc yMoveLeft*factorLeft + yMoveLeft2*factorLeft2 + yMoveRight*factorRight + yMoveRight2*factorRight2
dp4 r1.y, v3, c3                   ; calc yMoveBottom*factorBottom + yMoveBottom2*factorBottom2 + yMoveTop*factorTop + yMoveTop2*factorTop2


mad r0.y, v1.y, c0.x, v1.x         ; add factorSelf*yMoveSelf
add r0.y, r0.y, r1.x               ; add left & right factors
add r0.y, r0.y, r1.y               ; add bottom & top factors


;m4x4 oPos, r0, c4                  ; matrix transformation

m4x4 r3, r0, c4                  ; matrix transformation
mov oPos, r3


;------------------------------------------------------------------------------
; Texture coordinates
;------------------------------------------------------------------------------

; Calculate tex coords for pass 0
; (use texture matrix)
mov r4.xy, v0.zw
mov r4.zw, c0.xw
dp4 oT0.x,  r4, c8
dp4 oT0.y,  r4, c9

;dp4 oT0.x,  v0.z, c8
;dp4 oT0.y,  v0.w, c9


; Create tex coords for pass 1
; (simple copy)
mov oT1.xy, v0.zw


;------------------------------------------------------------------------------
; Fog calculation
;------------------------------------------------------------------------------

;mov oFog.x, r3.z
;mov oFog.x, c0.w
