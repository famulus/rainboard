round_over = 4;
wall_thickness = 2.5 ;

rainboard_diameter = 332.55  ; // point to point, long diagonal  288 short diag
rainboard_minor_diameter = 288;

rainboard_short_diameter = 280;
rainboard_short_minor_diameter = 242.5;



rainboard_height = 33;

hole_radius = 12;

resolution = 15;

back_panel = [125,40,60];

foot_inset = 15;

module footprint(){
  translate([0,11.4,0]) import("/Users/clean/Documents/rainboard/rainboard-chassis/thunder/chassis_solver.svg", center=true);


}

// ------------------ EXTERIOR ------------------

module mainBody(rainboard_height,rainboard_diameter, round_over ) {
  minkowski(){
    linear_extrude(height = rainboard_height, twist = 0, slices = 0) offset(delta=  (-wall_thickness)) 
    footprint();
    
    sphere(round_over);
  }
}


// ------------------ INTERIOR ------------------


module interior(rainboard_height,rainboard_diameter, round_over, wall_thickness ) {

  minkowski(){
    translate([0,0,wall_thickness])linear_extrude(height = 40, twist = 0, slices = 0) offset(delta=-(2*wall_thickness)) 
    footprint();
    
    sphere(round_over);
  }
}



//------------------ LEDGE ------------------


clip_thickness = 2.5;
clip_offset = clip_thickness/2;

module ledge( ) {    // ledge for plexiglass
  plexi_thickness = 3; 

  difference(){
    translate([0,0,rainboard_height-plexi_thickness-clip_thickness])linear_extrude(height = wall_thickness)
    footprint();

    translate([0,0,rainboard_height-plexi_thickness-5])linear_extrude(height = wall_thickness+10)
    offset(delta=-(2*wall_thickness))
    footprint();
  }


//------------------ CLIP ------------------


  module clip( ) {
    //clip
    translate([0,0,(rainboard_height+2.5)])
    rotate([65,0,0]) rotate([0,90,0])    cylinder(h=70 , d=(clip_thickness  ), center=true, $fn=3);   // the clip 
  }


  translate([0,(rainboard_minor_diameter/2)-1,0])
  clip();
  
  rotate([0,0,180]) 
  translate([0,(rainboard_short_minor_diameter/2)-1.2,0]) 
  clip();
  rotate([0,0,60]) 
  translate([0,(rainboard_short_minor_diameter/2)-1.2,0]) 
  clip();
  rotate([0,0,300]) 
  translate([0,(rainboard_short_minor_diameter/2)-1.2,0]) 
  clip();

}

// ------------------ BACK PANEL ------------------

module backPanel(dimentions=[1,2,3]) {

usb_width = 14.5;     //cube_width
usb_opening = [usb_width,usb_width,10] ;
    
    
top_of_chassis_to_top_of_pcb = 25;    
    
pcb_offset = top_of_chassis_to_top_of_pcb ;
 
translate([0,((rainboard_minor_diameter/2)-1.5),pcb_offset]) rotate([90,0,0]){ // place all holes at the back panel
    
     translate([ 0, -(usb_width/2),0]) cube(usb_opening, center=true); // cutout for USB
     }   
 }




// ------------------ FINAL ------------------


difference(){
  difference(){
    mainBody(rainboard_height,rainboard_diameter, round_over );                   
    interior(rainboard_height,rainboard_diameter, round_over, wall_thickness );
  };

  //    footer_cutout(rainboard_height,rainboard_diameter, foot_inset);


  buttons(buttons_coordinates);

     backPanel();


};



//      %mainBody(rainboard_height,rainboard_diameter, round_over );                   

// ------------------ LEDGE ------------------
ledge();
//#footprint();

// cylinder(h=rainboard_height-(wall_thickness*2), d=(rainboard_diameter- (wall_thickness*2)), center=true,$fn=6);     


module buttons(buttons ) {

  //    button    
  for(i = buttons){
    translate([i[0],i[1],-50])cylinder(r=12, h=100, $fn=resolution);
  };
}

buttons_coordinates =

[
[  -117.98730101159192,  0 ],
[  -103.23888838514293,  25.545 ],
[  -88.49047575869395,  51.09 ],
[  -73.74206313224495,  76.635 ],
[  -58.99365050579596,  102.18 ],
[  -103.23888838514293,  -25.545 ],
[  -88.49047575869395,  0 ],
[  -73.74206313224495,  25.545 ],
[  -58.99365050579596,  51.09 ],
[  -44.24523787934697,  76.635 ],
[  -29.49682525289798,  102.18 ],
[  -88.49047575869395,  -51.09 ],
[  -73.74206313224495,  -25.545 ],
[  -58.99365050579596,  0 ],
[  -44.24523787934697,  25.545 ],
[  -29.49682525289798,  51.09 ],
[  -14.74841262644899,  76.635 ],
[  0,  102.18 ],
[  -73.74206313224495,  -76.635 ],
[  -58.99365050579596,  -51.09 ],
[  -44.24523787934697,  -25.545 ],
[  -29.49682525289798,  0 ],
[  -14.74841262644899,  25.545 ],
[  0,  51.09 ],
[  14.74841262644899,  76.635 ],
[  29.49682525289798,  102.18 ],
[  -58.99365050579596,  -102.18 ],
[  -44.24523787934697,  -76.635 ],
[  -29.49682525289798,  -51.09 ],
[  -14.74841262644899,  -25.545 ],
[  0,  0 ],
[  14.74841262644899,  25.545 ],
[  29.49682525289798,  51.09 ],
[  44.24523787934697,  76.635 ],
[  58.99365050579596,  102.18 ],
[  -29.49682525289798,  -102.18 ],
[  -14.74841262644899,  -76.635 ],
[  0,  -51.09 ],
[  14.74841262644899,  -25.545 ],
[  29.49682525289798,  0 ],
[  44.24523787934697,  25.545 ],
[  58.99365050579596,  51.09 ],
[  73.74206313224495,  76.635 ],
[  0,  -102.18 ],
[  14.74841262644899,  -76.635 ],
[  29.49682525289798,  -51.09 ],
[  44.24523787934697,  -25.545 ],
[  58.99365050579596,  0 ],
[  73.74206313224495,  25.545 ],
[  88.49047575869395,  51.09 ],
[  29.49682525289798,  -102.18 ],
[  44.24523787934697,  -76.635 ],
[  58.99365050579596,  -51.09 ],
[  73.74206313224495,  -25.545 ],
[  88.49047575869395,  0 ],
[  103.23888838514293,  25.545 ],
[  58.99365050579596,  -102.18 ],
[  73.74206313224495,  -76.635 ],
[  88.49047575869395,  -51.09 ],
[  103.23888838514293,  -25.545 ],
[  117.98730101159192,  0 ]
];
