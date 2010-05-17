program tetris;

uses crt,DOS,graph256;
{egerforgatas,hejeskockakirakas,objectnemlathatoelek,arnyek a tablan}

Var map:array [0..128,0..128] of byte;
var aszo: byte;
    MAPPING : boolean;
    SHADOWS : boolean;

const camera : array[1..3] of real = (0,pi/12,pi/6);

const Am{rotates matrix} : array [1..6,1..4,1..4] of integer =
             ((( 0, 0, 1, 0),
               ( 0, 1, 0, 0),
               (-1, 0, 0, 0),
               ( 0, 0, 0, 1)),

              (( 0, 1, 0, 0),
               (-1, 0, 0, 0),
               ( 0, 0, 1, 0),
               ( 0, 0, 0, 1)),

              (( 1, 0, 0, 0),
               ( 0, 0, 1, 0),
               ( 0,-1, 0, 0),
               ( 0, 0, 0, 1)),

              (( 1, 0, 0, 0),
               ( 0, 1, 0, 0),
               ( 0, 0, 0, 1),
               ( 0, 0,-1, 0)),

              (( 1, 0, 0, 0),
               ( 0, 0, 0, 1),
               ( 0, 0, 1, 0),
               ( 0,-1, 0, 0)),

              (( 0, 0, 0, 1),
               ( 0, 1, 0, 0),
               ( 0, 0, 1, 0),
               (-1, 0, 0, 0)));

type pont = object
              x : array[1..4] of integer;
            end;

type vector =object
              x : array[1..3] of real;
            end;


Const blockconst : array [0..5,1..4,1..4] of integer=
(((0,0,0,0),(0,0,0,0),(0,0,0,0),(0,0,0,0)),
 ((0,0,0,0),(1,0,0,0),(0,0,0,0),(0,0,0,0)),
 ((0,0,0,0),(1,0,0,0),(0,1,0,0),(0,0,0,0)),
 ((0,0,0,0),(0,0,0,0),(0,0,0,0),(0,0,0,0)),
 ((0,0,0,0),(1,0,0,0),(0,1,0,0),(0,0,1,0)),
 ((0,1,0,0),(0,0,0,0),(1,0,0,0),(1,1,0,0)));

Const points : array[1..8,1..3] of real =
  ((1,1,1),(1,1,-1),(1,-1,-1),(1,-1,1),
  (-1,1,1),(-1,1,-1),(-1,-1,-1),(-1,-1,1));
        Flats: array[1..6,1..4] of byte =
  ((1,2,3,4),(5,6,7,8),(1,2,6,5),(3,4,8,7),(1,4,8,5),(2,3,7,6));

Const Width : byte = 2;

type blocktype = object
              coord : array [1..4] of pont;
              Ocoord : pont;
              PresentCoord : pont;
              Procedure rotate(tengely,LorR:integer);
              Procedure move(tengely,LorR:integer);
              Procedure down(UorD:integer);
              Procedure draw(EorF : integer);
            end;

type tabletype = object
              Player : byte;
              score : longint;
              table : array [0..3,0..3,0..3,0..10] of byte;
              Blocks : array [0..5] of blocktype;
              block : blocktype;
              function nogood(i : integer):boolean;
              procedure Tableinit(playernumber:byte);
              procedure BlocksInit;
              procedure new;
              function leert(i : integer):boolean;
              function felert(i: integer):boolean;
              procedure Sorkitolt;
              procedure draw;
              procedure lerak;
              Procedure putscore;
            end;

type cube = object
              col : byte;
              quadrant: array[1..4,1..2] of integer;
              xmap: integer;
              ymap: integer;
              light:vector;
              points2 : array[1..2,1..8] of vector;
              procedure turn(var x,y : real;fi : real);
              Procedure line3d(x1,y1,z1,x2,y2,z2 : real;c:byte);
              Function Visible(num:byte;szor:real):boolean;
              Procedure putcube(x1,x2,x3,x4,EorF,BorT : integer);
              Procedure rollcamera(tengely,LorR : integer);
              procedure fillquad;
            end;

function pow(n1:integer;n2 : integer):integer;
    var npow: integer;
        popo: integer;
  begin
    popo:=1;
    for npoW:=1 to n2 do popo:=popo*n1;
    if n2=0 then popo:=1;
    pow:=popo;
  end;

procedure mapgenerator;
     var x1p,y1p,tav,szam,i,x1,y1,x2,y2:integer;
     var c,x,y: integer;
     i2,j,e : real;

   begin
      clrscr;
      Write('Map generating ');
      For x:=0 to 128 do For Y:=0 to 128 do Map[x,y]:=128;
      For i:=1 to 7 do begin
        Write('.');
        tav:=poW(2,7-i);
        szam:=poW(2,i);
        For X1:=0 to szam do For Y1:=0 to szam do begin
          x1p:=x1*tav;y1p:=y1*tav;
          Map[x1p,y1p]:=Map[x1p,Y1p]+Random(trunc(256/szam))-trunc((128/szam));
          If (x1>0) and (y1>0) then begin
            For X2:=0 to tav do Map[X1P-tav+X2,Y1P-tav]:=trunc
             (((Map[x1P,Y1P-tav]-Map[x1P-tav,Y1P-tav])*x2)/tav)+Map[x1P-tav,Y1P-tav];
            For X2:=0 to tav do Map[X1P-tav+X2,Y1P]:=trunc
             (((Map[x1P,Y1P]-Map[x1P-tav,Y1P])*x2)/tav)+Map[x1P-tav,Y1P];
            For X2:=0 to tav do begin
              For Y2:=0 to tav do begin
                 Map[X1P-tav+X2,Y1P-tav+Y2]:=trunc
                 (((Map[X1P-tav+X2,Y1P]-Map[X1P-tav+X2,Y1P-tav])*Y2)/tav)
                 +Map[X1P-tav+X2,Y1P-tav]
end;end;end;end;end;
  write('OK');
  for x:=0 to 128 do for y:=0 to 128 do begin
    i2:=2*pi/200*x;
    j:=2*pi/200*y;
    e:=((cos(i2))+sqr(cos(2*j-i2)))*(cos(i2)*(sin(i2)*(j/2-j))-(2*sin(j)*cos(i2+2*j)))
    +cos(i2-2*sin(j))/2;
    c:=round((10*e)/5);
    if trunc(c) mod 2 = 1 then map[x,y]:=map[x,y]+8;
  end;
  for x:=0 to 128 do for y:=0 to 128 do map[x,y]:=trunc(map[x,y]/32);
end;

procedure Min(A,B:vector;var C:vector);
begin
  C.x[1]:=b.x[1]-a.x[1];
  C.x[2]:=b.x[2]-a.x[2];
  C.x[3]:=b.x[3]-a.x[3];
end;


Procedure X(A,B:vector;var C:vector);
begin
   C.x[1]:=a.x[2]*b.x[3]-b.x[2]*a.x[3];
   C.x[2]:=a.x[3]*b.x[1]-b.x[3]*a.x[1];
   C.x[3]:=a.x[1]*b.x[2]-b.x[1]*a.x[2];
end;

Function ABSV(A:vector):real;
   begin
      AbsV:=sqrt(sqr(A.x[1])+sqr(A.x[2])+sqr(A.x[3]));
   end;

Function bezartcos(A,B:vector):real;
   begin
    bezartcos:=(A.x[1]*B.x[1]+A.x[2]*B.x[2]+A.x[3]*B.x[3])/(AbsV(A)*AbsV(B));
   end;

function Tricolor(p1,p2,p3,light: vector):byte;
   var A,B,N : vector;
       cosx:real;
begin
   min(p2,p1,A);
   min(p2,p3,B);
   X(A,B,N);
   Tricolor:=trunc((1-abs(bezartcos(N,Light)))*8);
end;


procedure pset2(xo,yo,c:integer);
   var  ossz:longint;
   cim:longint;
   ablakszam:byte;
BEGIN
   ossz:=yo;
   ossz:=ossz*640;
   inc(ossz,xo);
   cim:=ossz mod 65536;
   ablakszam:=ossz div 65536;
   if ablakszam<>aszo then window(ablakszam);
   mem[$A000:cim]:=c ;
   aszo:=ablakszam;
end;

procedure Cube.fillquad;
  var A:array[1..4,1..3] of integer;
      V:array[1..3,1..2] of integer;
      x,y,xo,x1,yo,y1,i,j,szamol,c:integer;
      a1,a2,b1,b2,b3:real;
begin
  xmap:=128; Ymap:=128;
  V[1,1]:=quadrant[2,1]-quadrant[1,1];  V[1,2]:=quadrant[2,2]-quadrant[1,2];
  V[2,1]:=quadrant[4,1]-quadrant[1,1];  V[2,2]:=quadrant[4,2]-quadrant[1,2];
  b1:=V[1,1]/V[1,2];
  b2:=1/(V[2,1]-b1*V[2,2]);
  b3:=1/V[1,2];

  for i:=1 to 4 do begin
    if i<4 then j:=i+1 else j:=1;
    A[i,1]:=quadrant[i,2]-quadrant[j,2];
    A[i,2]:=quadrant[i,1]-quadrant[j,1];
    A[i,3]:=quadrant[i,1]*quadrant[j,2]-quadrant[i,2]*quadrant[j,1];
  end;
  x1:=0;y1:=0;  xo:=640;yo:=480;
  for i:=1 to 4 do if quadrant[i,1]>x1 then x1:=quadrant[i,1];
  for i:=1 to 4 do if quadrant[i,2]>y1 then y1:=quadrant[i,2];
  for i:=1 to 4 do if quadrant[i,1]<xo then xo:=quadrant[i,1];
  for i:=1 to 4 do if quadrant[i,2]<yo then yo:=quadrant[i,2];
  for y:=yo to y1 do for x:=xo to x1 do begin
     szamol:=0;
     for i:=1 to 4 do if x*A[i,1]-y*A[i,2]+A[i,3]<0 then szamol:=szamol+1;
     if szamol=4 then begin
     if mapping then begin
               V[3,1]:=x-quadrant[1,1];       V[3,2]:=y-quadrant[1,2];
          a2:=(V[3,1]-b1*V[3,2])*b2;     a1:=(V[3,2]-a2*V[2,2])*b3;
          c:=Map[trunc(a2*xmap),trunc(a1*ymap)];
     end else c:=5;
        pset2(x,y,col+c);
     end;
  end;
end;

Procedure palette;
   var i,arny:byte;
 BEGIN
   for i:=0 to 15 do begin
      setpal(16+i,i*4,0,0);      setpal(32+i,0,i*4,0);
      setpal(48+i,0,0,i*4);      setpal(64+i,0,i*4,i*4);
      setpal(80+i,i*4,0,i*4);    setpal(96+i,i*4,i*4,0);
      setpal(112+i,i*4,0,0);     setpal(128+i,0,i*4,0);
      setpal(144+i,0,0,i*4);     setpal(160+i,0,i*4,i*4);
      setpal(176+i,i*4,i*4,i*4);
      setpal(i,0,0,0);
   end;
 end;

procedure cleardevice;
   var i,h:integer;
       x:word;
begin
   for i:=0 to 4 do begin
     window(i);
     for x:=0 to 65535 do mem[$A000:x]:=0;
   end;
end;

Procedure line(x1,y1,x2,y2,c: word);
   var i: integer;
       ax,ay: integer;
       axd,ayd:real;
       d:integer;
       xo,yo:integer;
   ossz:longint;
   cim:longint;
   ablakszam,asz2:byte;
begin
   ax:=x2-x1;
   ay:=y2-y1;
   d:=trunc(sqrt(ax*ax+ay*ay));
   if d<>0 then
       for i:=0 to  d do begin
          xo:=x1+trunc((i*ax)/d);
          yo:=y1+trunc((i*ay)/d);
          Pset2(xo,yo,c);
       end;
end;

procedure cube.rollcamera(tengely,LorR : integer);
   begin
     cleardevice;
     camera[tengely]:=camera[tengely]+((LorR*pi)/24);
   end;

procedure cube.turn(var x,y : real;fi : real);
     var y1,x1,z1 : real;
         k : integer;
  begin
    if (x<0) then k:=-1 else k:=1;
    if x<>0 then begin
    x1:= sqrt(sqr(x)+sqr(y))*k*cos(arctan(y/x)+(fi));
    y1:= sqrt(sqr(x)+sqr(y))*k*sin(arctan(y/x)+(fi));
    end else if y>0 then begin
    x1:=cos(pi/2+fi)*y;
    y1:=sin(pi/2+fi)*y;
    end else Begin
    x1:=cos(3*pi/2+fi)*abs(y);
    y1:=sin(3*pi/2+fi)*abs(y);
    end;
    x:=x1;
    y:=y1;
  end;

Procedure cube.line3d(x1,y1,z1,x2,y2,z2 : real;c:byte);
    begin
        if (z1<>(-40)) and (z2<>(-40)) then
       line(320+trunc(1000*x1/(z1+40)),240+trunc(1000*y1/(z1+40)),
       320+trunc(1000*x2/(z2+40)),240+trunc(1000*y2/(z2+40)),c);
    end;


Function cube.Visible(num:byte;szor:real):boolean;
     var Vect: array[1..3,1..3] of real;
         sujp: array[1..3] of real;
     n,i:integer;
  BEGIN
     for n:=1 to 3 do
        sujp[n]:=(points2[1,1].x[n]+points2[1,3].x[n]+
                  points2[1,2].x[n]+points2[1,4].x[n]+
                  points2[1,5].x[n]+points2[1,7].x[n]+
                  points2[1,6].x[n]+points2[1,8].x[n])/8;
     i:=num;
     for n:=1 to 3 do Vect[1,n]:=points2[1,flats[i,2]].x[n]-points2[1,flats[i,1]].x[n];
     for n:=1 to 3 do Vect[2,n]:=points2[1,flats[i,3]].x[n]-points2[1,flats[i,1]].x[n];
     Vect[3,1]:=(Vect[1,2]*Vect[2,3]-Vect[1,3]*Vect[2,2])/(2*szor*2*szor);
     Vect[3,2]:=(Vect[1,3]*Vect[2,1]-Vect[1,1]*Vect[2,3])/(2*szor*2*szor);
     Vect[3,3]:=(Vect[1,1]*Vect[2,2]-Vect[1,2]*Vect[2,1])/(2*szor*2*szor);
     {kockanal}
     if sqrt(sqr(vect[3,1]+points2[1,flats[i,1]].x[1]-sujp[1])
            +sqr(vect[3,2]+points2[1,flats[i,1]].x[2]-sujp[2])
            +sqr(vect[3,3]+points2[1,flats[i,1]].x[3]-sujp[3]))<
        sqrt(sqr(points2[1,flats[i,1]].x[1]-sujp[1])
            +sqr(points2[1,flats[i,1]].x[2]-sujp[2])
            +sqr(points2[1,flats[i,1]].x[3]-sujp[3]))
      then begin
       vect[3,1]:=-vect[3,1];
       vect[3,2]:=-vect[3,2];
       vect[3,3]:=-vect[3,3];
     end;

     If (Vect[3,3]/sqrt(sqr(vect[3,1])+sqr(vect[3,2])+sqr(vect[3,3])))<(-(2*szor)/40)
     then Visible:=true else Visible:=false;
  End;

Procedure cube.putcube(x1,x2,x3,x4,EorF,BorT : integer);
    var n,i,o:integer;
        szor1,szor2:real;
        setcol,col2:byte;
  BEGIN
     light.x[1]:=15;     light.x[2]:=20;     light.x[3]:=10;
     col2:=(x4+1)*16;
     szor1:=18/(18-(x4+0.5));
     szor2:=18/(18-(x4-0.5));
     For i:=1 to 8 do begin
        points2[1,i].x[1]:=(points[i,1]-1+((x1-1)*2))*szor1;
        points2[1,i].x[2]:=(points[i,2]-1+((x2-1)*2))*szor1;
        points2[1,i].x[3]:=(points[i,3]-1+((x3-1)*2))*szor1;
        points2[2,i].x[1]:=(points[i,1]-1+((x1-1)*2))*szor2;
        points2[2,i].x[2]:=(points[i,2]-1+((x2-1)*2))*szor2;
        points2[2,i].x[3]:=(points[i,3]-1+((x3-1)*2))*szor2;
     end;

     For i:=1 to 8 do begin
        turn(points2[1,i].x[1],points2[1,i].x[2],camera[1]);
        turn(points2[2,i].x[1],points2[2,i].x[2],camera[1]);
        turn(points2[1,i].x[3],points2[1,i].x[2],camera[2]);
        turn(points2[2,i].x[3],points2[2,i].x[2],camera[2]);
        turn(points2[1,i].x[1],points2[1,i].x[3],camera[3]);
        turn(points2[2,i].x[1],points2[2,i].x[3],camera[3]);
     end;
     if BorT=0 then begin
     For i:=1 to 6 do begin
       if EorF=1 then setcol:=187 else setcol:=0;
       for n:=1 to 3 do begin
        line3d(points2[1,Flats[i,n]].x[1],points2[1,Flats[i,n]].x[2],points2[1,Flats[i,n]].x[3],
            points2[1,Flats[i,n+1]].x[1],points2[1,Flats[i,n+1]].x[2],points2[1,Flats[i,n+1]].x[3],setcol);
        line3d(points2[2,Flats[i,n]].x[1],points2[2,Flats[i,n]].x[2],points2[2,Flats[i,n]].x[3],
            points2[2,Flats[i,n+1]].x[1],points2[2,Flats[i,n+1]].x[2],points2[2,Flats[i,n+1]].x[3],setcol);
        end;
        line3d(points2[1,Flats[i,4]].x[1],points2[1,Flats[i,4]].x[2],points2[1,Flats[i,4]].x[3],
            points2[1,Flats[i,1]].x[1],points2[1,Flats[i,1]].x[2],points2[1,Flats[i,1]].x[3],setcol);
        line3d(points2[2,Flats[i,4]].x[1],points2[2,Flats[i,4]].x[2],points2[2,Flats[i,4]].x[3],
            points2[2,Flats[i,1]].x[1],points2[2,Flats[i,1]].x[2],points2[2,Flats[i,1]].x[3],setcol);
   end;
   for i:=1 to 8 do line3d(points2[1,i].x[1],points2[1,i].x[2],points2[1,i].x[3],
                           points2[2,i].x[1],points2[2,i].x[2],points2[2,i].x[3],setcol);
   end else begin
     setcol:=(x4+1)*16+15;
     for i:=1 to 6 do begin
          If Visible(i,szor1)=true then begin
             for o:=1 to 4 do begin
               if (points2[1,flats[i,o]].x[3]<>(-40)) then begin
                 quadrant[o,1]:=320+trunc(1000*points2[1,flats[i,o]].x[1]/(points2[1,flats[i,o]].x[3]+40));
                 quadrant[o,2]:=240+trunc(1000*points2[1,flats[i,o]].x[2]/(points2[1,flats[i,o]].x[3]+40));
               end;
             end;
             if shadows then col:=col2+abs(tricolor(points2[1,flats[i,1]],points2[1,flats[i,2]],
                                     points2[1,flats[i,3]],Light))
                        else col:=col2+5;
             Fillquad;
             for n:=1 to 3 do begin
                line3d(points2[1,Flats[i,n]].x[1],points2[1,Flats[i,n]].x[2],points2[1,Flats[i,n]].x[3],
                       points2[1,Flats[i,n+1]].x[1],points2[1,Flats[i,n+1]].x[2],points2[1,Flats[i,n+1]].x[3],setcol);
             end;
             line3d(points2[1,Flats[i,4]].x[1],points2[1,Flats[i,4]].x[2],points2[1,Flats[i,4]].x[3],
                    points2[1,Flats[i,1]].x[1],points2[1,Flats[i,1]].x[2],points2[1,Flats[i,1]].x[3],setcol);
          end;
     end;
   end;
End;


Procedure blocktype.rotate(tengely,LorR:integer);{left=-1 right=1}
      var n,n1,n2: byte;
          temp: array [1..4] of real;
          temp2: array [1..4] of real;
          LorR2:integer;
   begin
      for n:=1 to 4 do begin
        for n1:=1 to 4 do temp[n1]:=coord[n].x[n1]-0.5;
        for n1:=1 to 4 do begin
          temp2[n1]:=0;
          for n2:=1 to 4 do begin
           If n1=n2 then LorR2:=1 else LorR2:=LorR;
           temp2[n1]:=temp2[n1]+(LorR2*Am[tengely,n1,n2]*(temp[n2]));
          end;
        end;
        for n1:=1 to 4 do coord[n].x[n1]:=trunc(temp2[n1]+0.5);
      end;
   end;

Procedure blocktype.move(tengely,LorR:integer);
      var n: byte;
          bad:boolean;
   begin
     bad:=false;
     Presentcoord.x[tengely]:=Presentcoord.x[tengely]+LorR;
   end;

Procedure blocktype.down(UorD:integer);
   begin
     Presentcoord.x[4]:=Presentcoord.x[4]+UorD;
   end;

Procedure blocktype.draw(EorF : integer);
     var n : byte;
         c : cube;
   begin
     For n:=1 to 4 do c.Putcube(Presentcoord.x[1]+coord[n].x[1],
                              Presentcoord.x[2]+coord[n].x[2],
                              Presentcoord.x[3]+coord[n].x[3],
                              Presentcoord.x[4]+coord[n].x[4],
                              EorF,0);
  end;

procedure tabletype.Tableinit(playernumber:byte);
     var i1,i2,i3,i4:byte;
   begin
     player:=playernumber;
     score:=0;
     for i1:=0 to 3 do
        for i2:=0 to 3 do
           for i3:=0 to 3 do
              for i4:=0 to 10 do table[i1,i2,i3,i4]:=1;
     for i1:=1 to 2 do
        for i2:=1 to 2 do
           for i3:=1 to 2 do
              for i4:=1 to 10 do table[i1,i2,i3,i4]:=0;
   end;

Procedure tabletype.BlocksInit;
     var i,n : integer;
         stock:file of integer;
   begin
{     assign(stock,'blocks.dat');
     reset(stock);
 }    For n:=0 to 5 do begin
       blocks[n].Presentcoord.x[1]:=1;
       blocks[n].Presentcoord.x[2]:=1;
       blocks[n].Presentcoord.x[3]:=1;
       blocks[n].Presentcoord.x[4]:=9;

       For i:=1 to 4 do begin
         blocks[n].coord[i].x[1]:=Blockconst[n,i,1];
         blocks[n].coord[i].x[2]:=Blockconst[n,i,2];
         blocks[n].coord[i].x[3]:=Blockconst[n,i,3];
         blocks[n].coord[i].x[4]:=Blockconst[n,i,4];
       end;
       blocks[n].Ocoord.x[1]:=0;
       blocks[n].Ocoord.x[2]:=0;
       blocks[n].Ocoord.x[3]:=0;
       blocks[n].Ocoord.x[4]:=9;
     end;
   end;

procedure tabletype.new;
   begin
      Block:=Blocks[trunc(random(6))];
   end;

function tabletype.nogood(i : integer):boolean;
     var n:integer;
   Begin
     nogood:=true;
     for n:=1 to 4 do if table[block.Presentcoord.x[1]+block.coord[n].x[1],
                               block.Presentcoord.x[2]+block.coord[n].x[2],
                               block.Presentcoord.x[3]+block.coord[n].x[3],
                               block.Presentcoord.x[4]+block.coord[n].x[4]]=1
                            then nogood:=false;
     {ha false, nem johet letre a move}
    end;

function tabletype.leert(i : integer):boolean;
      var n:integer;
   begin
      Block.Down(-1);
      if noGood(1)=false then leert:=true else leert:=false;
      Block.Down(1);
   end;

function tabletype.felert(i: integer):boolean;
      var i1,i2,i3,i4,n:integer;
   begin
      felert:=false;
     for i1:=1 to 2 do
        for i2:=1 to 2 do
           for i3:=1 to 2 do
              if table[i1,i2,i3,9]=1 then felert:=true;
   end;

procedure tabletype.Sorkitolt;
      var szamlal,sorok,i1,i2,i3,i4,xi,yj,n : integer;
   begin
      sorok:=0;
      for i4:=1 to 10 do begin
         szamlal:=0;
         for i1:=1 to 2 do
           for i2:=1 to 2 do
              for i3:=1 to 2 do
                if table[i1,i2,i3,i4]=1 then szamlal:=szamlal+1;
         if szamlal=8 then begin
           For yj:=i4 to 9 do
             for i1:=1 to 2 do
               for i2:=1 to 2 do
                  for i3:=1 to 2 do Table[i1,i2,i3,yj]:=table[i1,i2,i3,yj+1];
           For i1:=1 to 2 do
             for i2:=1 to 2 do
               for i3:=1 to 2 do Table[i1,i2,i3,10]:=0;
           i4:=i4-1;
           score:=score+(width*width*width);
           Sorok:=sorok+1;
         end;
      end;
      score:=score+trunc(((sorok)*width*width*width)/2)
   end;

procedure tabletype.lerak;
     var n:integer;
   Begin
     for n:=1 to 4 do table[block.Presentcoord.x[1]+block.coord[n].x[1],
                            block.Presentcoord.x[2]+block.coord[n].x[2],
                            block.Presentcoord.x[3]+block.coord[n].x[3],
                            block.Presentcoord.x[4]+block.coord[n].x[4]]:=1;
   end;

procedure tabletype.draw;
      var i1,i2,i3,i4,n,j:integer;
          c: cube;
          wisibl : array [1..2,1..2,1..2] of integer;
   begin
   cleardevice;
       For i1:=1 to 2 do
         for i2:=1 to 2 do
           for i3:=1 to 2 do wisibl[i1,i2,i3]:=0;
     For i4:=0  to 10 do
       For i1:=1 to 2 do
         for i2:=1 to 2 do
           for i3:=1 to 2 do
             if table[i1,i2,i3,i4]=1 then wisibl[i1,i2,i3]:=i4;
       For i3:=2 downto 1 do
         for i1:=2 downto 1 do
           for i2:=1 to 2 do
             For i4:=0  to 10 do
               if wisibl[i1,i2,i3]=i4 then c.putcube(i1,i2,i3,i4,1,1);
     block.draw(1);
   end;

Procedure Tabletype.putscore;
      var s:string;
   begin
  {    str(score,s);
      setfillstyle(1,0);
      Bar(600,220,639,260);
      OuttextXY(610,230,'Score :');
      Outtextxy(610,250,s);
 }  end;

Procedure graph_on;
  begin
    on;
  end;

Procedure initgam;
begin
   shadows:=true;
   mapping:=false;
end;

   var A : tabletype;
       ch : char;
       h,m,s0,s,hund : word;
       c:cube;

BEGIN
   mapgenerator;
   initgam;
   Randomize;
   GetTime(h,m,s,hund);
   s0:=h*3600+60*m+s;
   graph_on;
   palette;
   A.tableinit(1);
   A.BlocksInit;
   A.new;
   cleardevice;
   A.draw;
   A.block.draw(1);
   Repeat
     if keypressed then begin
     ch:=readkey;
     Case ch of
       'q' : begin
{             A.Block.draw(-1);
}            A.Block.move(1,1);
             if (not A.nogood(1)) then A.Block.move(1,-1);
     a.draw;
          end;
      'a'  : begin
{             A.Block.draw(-1);
 }            A.Block.move(1,-1);
             if (not A.nogood(1)) then A.Block.move(1,1);
     a.draw;
          end;
       'w' : begin
  {           A.Block.draw(-1);
   }          A.Block.move(2,1);
             if (not A.nogood(1)) then A.Block.move(2,-1);
     a.draw;
          end;
      's'  : begin
    {         A.Block.draw(-1);
     }        A.Block.move(2,-1);
             if (not A.nogood(1)) then A.Block.move(2,1);
     a.draw;
          end;
       'e' : begin
      {       A.Block.draw(-1);
       }      A.Block.move(3,1);
             if (not A.nogood(1)) then A.Block.move(3,-1);
     a.draw;
          end;
      'd'  : begin
        {     A.Block.draw(-1);
         }    A.Block.move(3,-1);
             if (not A.nogood(1)) then A.Block.move(3,1);
     a.draw;
          end;
      'r'  : begin
          {   A.Block.draw(-1);
           }  A.Block.rotate(1,1);
             if (not A.nogood(1)) then A.Block.rotate(1,-1);
     a.draw;
          end;
      'f'  : begin
            { A.Block.draw(-1);
             }A.Block.rotate(2,1);
             if (not A.nogood(1)) then A.Block.rotate(2,-1);
     a.draw;
          end;
      'v'  : begin
{             A.Block.draw(-1);
 }            A.Block.rotate(3,1);
             if (not A.nogood(1)) then A.Block.rotate(3,-1);
     a.draw;
          end;
      't'  : begin
  {           A.Block.draw(-1);
   }          A.Block.rotate(4,1);
             if (not A.nogood(1)) then A.Block.rotate(4,-1);
     a.draw;
          end;
      'g'  : begin
    {         A.Block.draw(-1);
     }        A.Block.rotate(5,1);
             if (not A.nogood(1)) then A.Block.rotate(5,-1);
     a.draw;
          end;
      'b'  : begin
      {       A.Block.draw(-1);
       }      A.Block.rotate(6,1);
             if (not A.nogood(1)) then A.Block.rotate(6,-1);
             a.draw;
          end;
      'y'  : begin
        {     A.Block.draw(-1);
         }    C.rollcamera(1,1);
             if (not A.nogood(1)) then C.rollcamera(1,-1);
             a.draw;
          end;
      'u'  : begin
          {   A.Block.draw(-1);
           }  C.rollcamera(1,-1);
             if (not A.nogood(1)) then C.rollcamera(1,1);
             a.draw;
          end;
      'h'  : begin
            { A.Block.draw(-1);
             }C.rollcamera(2,1);
             if (not A.nogood(1)) then C.rollcamera(2,-1);
             a.draw;
          end;
      'j'  : begin
{             A.Block.draw(-1);
 }            C.rollcamera(2,-1);
             if (not A.nogood(1)) then C.rollcamera(2,1);
             a.draw;
          end;
      'n'  : begin
  {           A.Block.draw(-1);
   }          C.rollcamera(3,1);
             if (not A.nogood(1)) then C.rollcamera(3,-1);
             a.draw;
          end;
      'm'  : begin
    {         A.Block.draw(-1);
     }        C.rollcamera(3,-1);
             if (not A.nogood(1)) then C.rollcamera(3,1);
             a.draw;
          end;
      ' '  : begin
             repeat
{               A.block.draw(-1);}
               A.block.down(-1);
{               A.block.draw(1);   }
             until A.leert(1);
          end;
       end;
     end;
     GetTime(h,m,s,hund);
     s:=h*3600+60*m+s;
     if not A.leert(1) then
     if (s-10)>s0 then begin
{       A.block.Draw(-1);
 }      A.block.Down(-1);
       a.draw;
       s0:=s;
     end;
     if A.leert(1) then begin
        A.lerak;
        A.new;
        A.Sorkitolt;
        A.PutScore;
        cleardevice;
        A.draw;
     end;
{     a.draw;}
   Until (A.felert(1));
End.