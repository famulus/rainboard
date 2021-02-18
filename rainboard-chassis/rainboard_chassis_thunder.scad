round_over = 4;
wall_thickness = 3 ;

rainboard_diameter = 332.55  ; // point to point, long diagonal  288 short diag
rainboard_minor_diameter = 288;

rainboard_height = 36;

hole_radius = 12;

resolution = 20;

back_panel = [125,40,60];

foot_inset = 15;



    
    
module backPanel(dimentions=[1,2,3]) {

usb_width = 14.5;     //cube_width
usb_opening = [usb_width,usb_width,10] ;
    
    
top_of_chassis_to_top_of_pcb = 25;    
    
pcb_offset = top_of_chassis_to_top_of_pcb -(rainboard_height/2);
 
translate([0,((288/2)-1.5),pcb_offset]) rotate([90,0,0]){ // place all holes at the back panel
    
     translate([ 0, -(usb_width/2),0]) cube(usb_opening, center=true); // cutout for USB
     }   
 }


module mainBody(rainboard_height,rainboard_diameter, round_over ) {

  minkowski(){
            
  //main body
        cylinder(h=rainboard_height-(round_over*2), d=(rainboard_diameter- (round_over*2)), center=true,$fn=6);   
    //round over
        sphere(round_over, $fn=resolution);
    
    }
 }


module interior(rainboard_height,rainboard_diameter, round_over, wall_thickness ) {

//  interior

    minkowski(){
    
        // interior hex
        cylinder(h=rainboard_height-(round_over*2)-(wall_thickness*2) , d=((rainboard_diameter- (round_over*2)) -(wall_thickness*2) ), center=true,$fn=6);     

        //  round over
        sphere(round_over, $fn=resolution);
        
    };

 }



module footer_cutout(rainboard_height,rainboard_diameter, foot_inset ) {
    translate([0,0,foot_inset])
    cylinder(h=rainboard_height+foot_inset , d=(rainboard_diameter-foot_inset), center=true,$fn=6);     


 }



module buttons(buttons ) {

//    button    
    for(i = buttons){
        translate([i[0],i[1],-50])cylinder(r=12, h=100, $fn=resolution);
    };
 }





module ledge( ) {    // ledge for plexiglass
    rainboard_center = rainboard_height/2;
    plexi_thickness = 3; 
    recess = 2; // space between bottom of plexi and bottom of chassis 
    ledge_thickness = 2;
    ledge_overhang = 4;
    clip_thickness = 2;
    clip_offset = clip_thickness/2;
    
    module clip( ) {
        //clip
        translate([0,-(rainboard_minor_diameter/2)+clip_offset+5.2,(rainboard_height/2)-clip_offset+0.0])
        rotate([43,0,0]) rotate([0,90,0])    cylinder(h=140 , d=(clip_thickness  ), center=true,$fn=3);   // the clip 

        }
    
clip();
rotate([0,0,180]) translate([0,0,00]) clip();
//             %6translate([0,0,  (rainboard_center - (plexi_thickness/2)-clip_thickness)  ])    cylinder(h=plexi_thickness , d=(rainboard_diameter  ), center=true,$fn=6);   // the acrylic 

    difference (){
         translate([0,0,  (rainboard_center - (ledge_thickness/2)-plexi_thickness-clip_thickness)  ])    cylinder(h=ledge_thickness , d=(rainboard_diameter  ), center=true,$fn=6);   // the ledge 
        
         translate([0,0,rainboard_center - 3-2])   cylinder(h=20 , d=(rainboard_diameter-foot_inset-ledge_overhang), center=true,$fn=6);     // the cutout negative


    }

 }





//  Final Output
 difference(){
    difference(){
      mainBody(rainboard_height,rainboard_diameter, round_over );                   
      interior(rainboard_height,rainboard_diameter, round_over, wall_thickness );
   };
            
    footer_cutout(rainboard_height,rainboard_diameter, foot_inset);
    
    backPanel(back_panel);
    
    buttons(buttons_coordinates);
    
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


//module pointy_hex(){
//    
//       translate([0,0,-rainboard_height/2]) hull(){
//          import("/Users/clean/Documents/rainboard/rainboard-chassis/chassis_solver.stl"); 
//           translate([0,0,rainboard_height-1])  import("/Users/clean/Documents/rainboard/rainboard-chassis/chassis_solver.stl");
//        }
//
//    }
//       

//pointy_hex();