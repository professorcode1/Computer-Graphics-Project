# Marching Cubes 
I have implimented the marching cubes algorithm to get a mesh from the formula(firstDump files being the results at different parameters). But
<li> It is running on the CPU so even multi-threaded its extremely slow.
<li> The results are somewhat good. But not as good. It looks extremely artificial.
<br>
Run the make command and then the main binary. It will create a file by the name of firstDump.OBJ of ~130 Mb. Use a website like https://www.creators3d.com/online-viewer to visualise the results. If on linux, you can use the f3d. 
<br>
In the main.cpp, in the scene constructor increase the marching cube lengths(last 6-3 parameters), though the bigger they become the more jagged and aritifical looking the result, but the smaller the file size.
<br> 
If i want to run it on the GPU I will probably have to write a geometry shader. And thats just not a task I am upto. I am switching to the UV Mapping Method instead. 
