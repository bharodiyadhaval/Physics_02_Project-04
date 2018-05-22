////////////////////////////////////////
Shalin Momin
Dhaval Bharodiya
///////////////////////////////////////
Key Presses:
Use these keys to move the ball:
W
A
S
D

Use these keys to rotate the camera:
Arrow Keys Left and Right

Use these Keys to Zoom in and Out while staying in the third person camera:
Arrow Keys UP and DOWN

Constraints Applied:
//////////////////////////

1. Ball socket joint, we've added a chain that hangs in mid air using a bunch of balls
2. Hinge, that's been made using a box shape that rotate on one of its corners using it as a pivot.
3. Slider, 2 sliders keep moving back and forth on X and Z axis.
4. Using the 6 DOF constraint, there is a cone which has been locked inside a virtual cube:
		from (-500,0,500) to (500,200,500)
There are 4 lights, lightening up the scene.
(And a pretty skybox too!)
