use <SnapLib.0.36.scad>


round_over = 4;
wall_thickness = 3 ;

rainboad_diameter = 332.55  ; // point to point, long diagonal  288 short diag
rainboard_height = 47;

hole_radius = 12;

resolution = 25;

back_panel = [125,40,60];

foot_inset = 15;






module backPanel(dimentions=[1,2,3]) {

midi_opening = 18.5; //diameter
usb_opening = [16,16,10] ;
usb_vert_offset = 9.03;    
    
dc_power_opening = 13; // diameter
    
midi_in_y = -15.4; // offset from 0 on PCB
midi_out_y = -37; 
midi_vert_offset = -8.25;   
 
translate([0,((288/2)-1.5),0]) rotate([90,0,0]){ // place all holes at the back panel
    
     translate([
    19.5+midi_in_y, usb_vert_offset,0]) cube(usb_opening, center=true); // cutout for USB
    translate([50+midi_in_y,usb_vert_offset,0]) cylinder(20, d=(dc_power_opening), center=true,$fn=resolution);  // cutout for power   

    translate([midi_in_y,midi_vert_offset,0]) cylinder(20, d=(midi_opening), center=true,$fn=resolution);   // midi IN  
    
    translate([midi_out_y,midi_vert_offset,0]) cylinder(20, d=(midi_opening), center=true,$fn=resolution);     // midi OUT


     }   
     

 }

















module mainBody(rainboard_height,rainboad_diameter, round_over ) {

  minkowski(){
            
//            main body
            
             cylinder(h=rainboard_height-(round_over*2), d=(rainboad_diameter- (round_over*2)), center=true,$fn=6);     
            
//            round over
            sphere(round_over, $fn=resolution);

            }

 }


module interior(rainboard_height,rainboad_diameter, round_over, wall_thickness ) {

//        interior
            
                   translate([0,0,0])
            minkowski(){
            
//            interior hex
//        cylinder(h=(rainboard_height+ wall_thickness), d=(rainboad_diameter - wall_thickness), center=true,$fn=6);
              cylinder(h=rainboard_height-(round_over*2)-(wall_thickness*2) , d=((rainboad_diameter- (round_over*2)) -(wall_thickness*2) ), center=true,$fn=6);     

            
//            round over
            sphere(round_over, $fn=resolution);

            };

 }

module footer_cutout(rainboard_height,rainboad_diameter, foot_inset ) {
        translate([0,0,foot_inset])
    cylinder(h=rainboard_height+foot_inset , d=(rainboad_diameter-foot_inset), center=true,$fn=6);     


 }






module buttons(buttons ) {

//    button
    
    
    for(i = buttons){
        translate([i[0],i[1],-50])cylinder(r=12, h=100, $fn=resolution);
    };
 }





module modulation_strips( ) {


//  https://cdn-shop.adafruit.com/product-files/178/SOFTPOT-DATA-SHEET-Rev-F3.pdf
tolerance = 1;
half_tolerance = tolerance /2;
ribbonLength =  115.86+ tolerance;
ribbonWidth = 20.32+ tolerance;
ribbonThickness = 0.58+ tolerance;
tailLength = 24.89+ tolerance;
tailWidth  = 10.16+ tolerance;    
distanceFromCenter =     126.9;
slot_height =     2.5;
active_area_width =   7.11;
  active_area_length =   100;  


difference(){
    
        cube([ribbonLength+(wall_thickness*2),ribbonWidth+(wall_thickness*2),slot_height+(wall_thickness+1)], true); // enclosing cuboid
    translate([0,0,1])cube([ribbonLength+20,ribbonWidth,slot_height], true); // slot for ribbon
          translate([0,0,10])cube([active_area_length,active_area_width,20], true); // smaller cutout for active area

    }


 }




module ledge( ) {

    // ledge for plexiglass
difference (){
 plexi_thickness = 3;   
    
 translate([0,0,rainboard_height/2 - (plexi_thickness/2)-plexi_thickness-1])    cylinder(h=plexi_thickness , d=(rainboad_diameter  ), center=true,$fn=6);   // the ledge  
    


    translate([0,0,rainboard_height/2 - 3-2])   cylinder(h=20 , d=(rainboad_diameter-foot_inset-4), center=true,$fn=6);     // the cutout negative

translate([0,((288/2)-1.5),0]) rotate([90,0,0]){ // place all holes at the back panel
    translate([20,(rainboard_height/2)-10.5,0])cube([80,10,20],true);

}

    }

    }




module sideButtons(buttons ) {

//    button
    for(i = [-2:2]){
        if(i != 0){
                  translate([i*29.5,-100,0]) rotate([90,0,0])  cylinder(r=12, h=100, $fn=resolution);
            }
   }
 }










//  Final Output
 
difference(){
    difference(){
      mainBody(rainboard_height,rainboad_diameter, round_over );                   
      interior(rainboard_height,rainboad_diameter, round_over, wall_thickness );
   };
            
    buttons(buttons_coordinates);
    backPanel(back_panel);

   rotate([0,0,60]) modulation_strips();
   
       rotate([0,0,60])sideButtons();
     rotate([0,0,300])sideButtons();

     rotate([0,0,0])sideButtons();
  
   footer_cutout(rainboard_height,rainboad_diameter, foot_inset);
backPanel();

};
    
ledge();




 
 



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

