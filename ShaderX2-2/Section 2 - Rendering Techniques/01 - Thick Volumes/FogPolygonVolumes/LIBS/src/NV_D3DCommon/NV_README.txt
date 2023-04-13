

TextDisplay
Has not been ported.  It is a long vertex shader.  Probably better to use d3d font class for rendering text.




ID3DXFont Interface

--------------------------------------------------------------------------------

The ID3DXFont interface is used to encapsulate the textures and resources needed to render a specific font on a specific device.

ID3DXFont Members

Begin Prepares a device for drawing text.
 
DrawText Draws formatted text. This method supports ANSI and Unicode strings.
 
End Restores the device state to how it was when ID3DXFont::Begin was called.
 
GetDevice Retrieves the Microsoft® Direct3D® device associated with the font object.
 
GetLogFont Retrieves the attributes of the font.
 
OnLostDevice Releases all references to video memory resources and deletes all stateblocks.
 
OnResetDevice Should be called after the device has been reset.
 

Remarks

The ID3DXFont interface is obtained by calling the D3DXCreateFont or D3DXCreateFontIndirect functions.

The ID3DXFont interface, like all Component Object Model (COM) interfaces, inherits the IUnknown interface methods.

The LPD3DXFONT type is defined as a pointer to the ID3DXFont interface.

