
round_over = 4;
wall_thickness = 3 ;

rainboad_diameter = 328  ;
rainboard_height = 41;

hole_radius = 12;

resolution = 200;

back_panel = [125,40,60];


module backPanel(dimentions=[1,2,3]) {

translate([0,130,20]) minkowski(){
    cube(back_panel, true);
sphere(round_over, $fn=resolution);
    };


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
            
                   translate([0,0,wall_thickness])
            minkowski(){
            
//            interior hex
//        cylinder(h=(rainboard_height+ wall_thickness), d=(rainboad_diameter - wall_thickness), center=true,$fn=6);
              cylinder(h=rainboard_height-(round_over*2), d=((rainboad_diameter- (round_over*2)) -(wall_thickness*2) ), center=true,$fn=6);     

            
//            round over
            sphere(round_over, $fn=resolution);

            };

 }


module buttons(buttons ) {

//    button
    
    
    for(i = buttons){
        translate([i[0],i[1],-50])cylinder(r=12, h=100, $fn=resolution);
    };
 }





module modulation_strips(depth ) {


//  https://cdn-shop.adafruit.com/product-files/178/SOFTPOT-DATA-SHEET-Rev-F3.pdf
tolerance = 0.12;
half_tolerance = tolerance /2;
ribbonLength =  115.86+ tolerance;
ribbonWidth = 20.32+ tolerance;
ribbonThickness = 0.58+ tolerance;
tailLength = 24.89+ tolerance;
tailWidth  = 10.16+ tolerance;    
distanceFromCenter =     126.9;

translate([0,distanceFromCenter,-(rainboard_height/2)]){
    cube([ribbonLength,ribbonWidth,ribbonThickness*2], true);
      
//    The Tail
 translate([ribbonLength/2,0,0])
       rotate([0,-25,0])

    cube([tailLength*2,tailWidth,ribbonThickness*2], true);
    
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
    modulation_strips(1);
   rotate([0,0,60]) modulation_strips(1);
//  rotate([0,0,120]) modulation_strips(1);
//   rotate([0,0,180]) modulation_strips(1);
//   rotate([0,0,240]) modulation_strips(1);
   rotate([0,0,300]) modulation_strips(1);
   
       rotate([0,0,60])sideButtons();
     rotate([0,0,300])sideButtons();
       rotate([0,0,240])sideButtons();

     rotate([0,0,0])sideButtons();

     rotate([0,0,120])sideButtons();

};
    






 



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

