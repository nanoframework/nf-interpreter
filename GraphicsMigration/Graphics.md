
##### Current progress in migration of graphics from netmf to nanoframework

Common low-level graphics/touch ink code from net-mf to nf-intepreter
<br>
![](CommonGraphics.png )

##### Low level display drivers

Copied net-mf display driver SSD1289 to nf-interpreter and altered to compile but not currently working or tested.
<br>
![](LowLevelDisplayDrivers.png )


### Configuration 

#### Code added to main CMakeLists.txt ( bottom)
 `User Interface (UI) code`<br>
`if(NF_FEATURE_UI_GRAPHICS)`<br>
   `add_subdirectory(src/CLR/UI/Graphics)`<br>
   `add_subdirectory("${PrimaryGraphicsController}")`<br>
`endif()`<br>

`if(NF_FEATURE_UI_TOUCH)`<br>
   `add_subdirectory("${PrimaryTouchController}")`<br>
`endif()`<br>

### Definitions added to CMakeSettings.json

![](CMakeSettingsJSON.png)

