Added the ability to use a specular map in the pixel shader, controlling per-pixel shininess.

The specular scaling I used is: pow(saturate(dot(dirToView, dirReflected)) * specularScalingValue, specExponent)

After trying various combinations, this is the formula I found to show per-pixel shininess the best, most visible on the cube in the scene. 

Tweaks/Things to note:
- I have only set one directional white light going in the (1,0,0) direction.
There are other lights, I have just set their intensity to 0, so that per-pixel specularity 
was clearly noticeable. 
- Fixed issues from previous assignment regarding the tweaking of light colors, done using 
coloredit3 instead of straight vectors now.