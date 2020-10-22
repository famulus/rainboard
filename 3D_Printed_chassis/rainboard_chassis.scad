round_over = 4;
wall_thickness = 3 ;

rainboad_diameter = 332.55  ; // point to point, long diagonal  288 short diag
rainboard_height = 48;

hole_radius = 12;

resolution = 100;

back_panel = [125,40,60];

foot_inset = 15;





module backPanel(dimentions=[1,2,3]) {

midi_opening = 18.5; //diameter
usb_opening = [14.5,14.5,10] ;
usb_vert_offset = 9.03;    
    
dc_power_opening = 13; // diameter
    
midi_in_y = -15.4; // offset from 0 on PCB
midi_out_y = -37; 
midi_vert_offset = -7.75;   
 
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





module ledge( ) {

    // ledge for plexiglass
difference (){
 plexi_thickness = 3;   
    
 translate([0,0,  (rainboard_height/2 - (plexi_thickness/2)-plexi_thickness-2)  ])    cylinder(h=plexi_thickness , d=(rainboad_diameter  ), center=true,$fn=6);   // the ledge  
    


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













module modulation_strips_positive( ) {


//  https://cdn-shop.adafruit.com/product-files/178/SOFTPOT-DATA-SHEET-Rev-F3.pdf
tolerance = 1;
half_tolerance = tolerance /2;
ribbonLength =  115.86+ tolerance;
ribbonWidth = 20.32+ tolerance;
ribbonThickness = 0.58+ tolerance;
tailLength = 24.89+ tolerance;
tailWidth  = 10.16+ tolerance;    
softpot_backer =     2;
softpot_wall_thickness = 1.5;


  rotate([0,180,0]) 
  translate([0,288/2-16.7,
    ((rainboard_height/2)-(softpot_wall_thickness*2))
    ])
   cube(
        [
            ribbonLength+(softpot_wall_thickness*2),
            ribbonWidth+(softpot_wall_thickness*2.5),
            softpot_backer+(softpot_wall_thickness)
        ]
        , true); // enclosing cuboid
        
        
      rotate([0,180,300]) 
  translate([0,288/2-16.7,
    ((rainboard_height/2)-(softpot_wall_thickness*2))
    ])
   cube(
        [
            ribbonLength+(softpot_wall_thickness*2),
            ribbonWidth+(softpot_wall_thickness*2.5),
            softpot_backer+(softpot_wall_thickness)
        ]
        , true); // enclosing cuboid
    
        
    }





module modulation_strips_negative( ) {


//  https://cdn-shop.adafruit.com/product-files/178/SOFTPOT-DATA-SHEET-Rev-F3.pdf
tolerance = 1;
half_tolerance = tolerance /2;
ribbonLength =  115.86+ tolerance;
ribbonWidth = 20.32+ tolerance;
ribbonThickness = 0.58+ tolerance;
tailLength = 24.89+ tolerance;
tailWidth  = 10.16+ tolerance;    
slot_height =     1.6;
slot_runout = 46;    
active_area_width =   15;
  active_area_length =   110;  
softpot_wall_thickness = 1.5;


  rotate([0,180,0]) 
  translate([0,288/2-16.7,
    ((rainboard_height/2)-(softpot_wall_thickness)-softpot_wall_thickness/2)
    ]){
        
        cube([ribbonLength+slot_runout,ribbonWidth,slot_height], true); // slot for ribbon

        translate([0,0,10])cube([active_area_length,active_area_width,20], true); // smaller cutout for active area
      }
      
      
  rotate([0,180,300]) 
  translate([0,288/2-16.7,
    ((rainboard_height/2)-(softpot_wall_thickness)-softpot_wall_thickness/2)
    ]){
        
        cube([ribbonLength+slot_runout,ribbonWidth,slot_height], true); // slot for ribbon

        translate([0,0,10])cube([active_area_length,active_area_width,20], true); // smaller cutout for active area
      }
      
      

//difference(){
//    
//
//
//    }


 }

















//  Final Output
difference(){
    difference(){
      mainBody(rainboard_height,rainboad_diameter, round_over );                   
      interior(rainboard_height,rainboad_diameter, round_over, wall_thickness );
   };
            
   footer_cutout(rainboard_height,rainboad_diameter, foot_inset);

    backPanel(back_panel);

   
     rotate([0,0,60])sideButtons();
     rotate([0,0,300])sideButtons();
     rotate([0,0,0])sideButtons();
      


    buttons(buttons_coordinates);

    backPanel();

   
      modulation_strips_negative();

};
    
ledge();


difference (){
    modulation_strips_positive();
      modulation_strips_negative();

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

