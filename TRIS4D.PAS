program tetris;

uses crt,DOS,graph;
{egerforgatas,hejeskockakirakas,objectnemlathatoelek,arnyek a tablan}

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
              points2 : array[1..2,1..8,1..3] of real;
              procedure turn(var x,y : real;fi : real);
              Procedure line3d(x1,y1,z1,x2,y2,z2 : real);
              Function Visible(num:byte;szor:real):boolean;
              Procedure putcube(x1,x2,x3,x4,EorF,BorT : integer);
              Procedure rollcamera(tengely,LorR : integer);
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
    x1:= sqrt(sqr(x)+sqr(y)) *k* cos( arctan(y/x)+(fi) );
    y1:= sqrt(sqr(x)+sqr(y)) *k* sin( arctan(y/x)+(fi) );
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

Procedure cube.line3d(x1,y1,z1,x2,y2,z2 : real);
    begin
        if (z1<>(-40)) and (z2<>(-40)) then
       line(320+trunc(1000*x1/(z1+40)),240+trunc(1000*y1/(z1+40)),
       320+trunc(1000*x2/(z2+40)),240+trunc(1000*y2/(z2+40)));
    end;


Function cube.Visible(num:byte;szor:real):boolean;
     var Vect: array[1..3,1..3] of real;
         sujp: array[1..3] of real;
     n,i:integer;
  BEGIN
     for n:=1 to 3 do
        sujp[n]:=(points2[1,1,n]+points2[1,3,n]+
                  points2[1,2,n]+points2[1,4,n]+
                  points2[1,5,n]+points2[1,7,n]+
                  points2[1,6,n]+points2[1,8,n])/8;
     i:=num;
     for n:=1 to 3 do Vect[1,n]:=points2[1,flats[i,2],n]-points2[1,flats[i,1],n];
     for n:=1 to 3 do Vect[2,n]:=points2[1,flats[i,3],n]-points2[1,flats[i,1],n];
     Vect[3,1]:=(Vect[1,2]*Vect[2,3]-Vect[1,3]*Vect[2,2])/(2*szor*2*szor);
     Vect[3,2]:=(Vect[1,3]*Vect[2,1]-Vect[1,1]*Vect[2,3])/(2*szor*2*szor);
     Vect[3,3]:=(Vect[1,1]*Vect[2,2]-Vect[1,2]*Vect[2,1])/(2*szor*2*szor);
     {kockanal}
     if sqrt(sqr(vect[3,1]+points2[1,flats[i,1],1]-sujp[1])
            +sqr(vect[3,2]+points2[1,flats[i,1],2]-sujp[2])
            +sqr(vect[3,3]+points2[1,flats[i,1],3]-sujp[3]))<
        sqrt(sqr(points2[1,flats[i,1],1]-sujp[1])
            +sqr(points2[1,flats[i,1],2]-sujp[2])
            +sqr(points2[1,flats[i,1],3]-sujp[3]))
      then begin
       vect[3,1]:=-vect[3,1];
       vect[3,2]:=-vect[3,2];
       vect[3,3]:=-vect[3,3];
     end;

     If (Vect[3,3]/sqrt(sqr(vect[3,1])+sqr(vect[3,2])+sqr(vect[3,3])))<(-(2*szor)/40)
     then Visible:=true else Visible:=false;
  End;

Procedure cube.putcube(x1,x2,x3,x4,EorF,BorT : integer);
    var quadrant: array[1..4] of PointType;
    var n,i,o:integer;
        szor1,szor2:real;
  BEGIN
     szor1:=15/(15-(x4+0.5));
     szor2:=15/(15-(x4-0.5));
     For i:=1 to 8 do begin
        points2[1,i,1]:=(points[i,1]-1+((x1-1)*2))*szor1;
        points2[1,i,2]:=(points[i,2]-1+((x2-1)*2))*szor1;
        points2[1,i,3]:=(points[i,3]-1+((x3-1)*2))*szor1;
        points2[2,i,1]:=(points[i,1]-1+((x1-1)*2))*szor2;
        points2[2,i,2]:=(points[i,2]-1+((x2-1)*2))*szor2;
        points2[2,i,3]:=(points[i,3]-1+((x3-1)*2))*szor2;
     end;

     For i:=1 to 8 do begin
        turn(points2[1,i,1],points2[1,i,2],camera[1]);
        turn(points2[2,i,1],points2[2,i,2],camera[1]);
        turn(points2[1,i,3],points2[1,i,2],camera[2]);
        turn(points2[2,i,3],points2[2,i,2],camera[2]);
        turn(points2[1,i,1],points2[1,i,3],camera[3]);
        turn(points2[2,i,1],points2[2,i,3],camera[3]);
     end;

     if BorT=0 then begin
     For i:=1 to 6 do begin
{       If Visible(i)=true then begin}
       if EorF=1 then setcolor(14) else setcolor(0);
       for n:=1 to 3 do begin
        line3d(points2[1,Flats[i,n],1],points2[1,Flats[i,n],2],points2[1,Flats[i,n],3],
            points2[1,Flats[i,n+1],1],points2[1,Flats[i,n+1],2],points2[1,Flats[i,n+1],3]);
        line3d(points2[2,Flats[i,n],1],points2[2,Flats[i,n],2],points2[2,Flats[i,n],3],
            points2[2,Flats[i,n+1],1],points2[2,Flats[i,n+1],2],points2[2,Flats[i,n+1],3]);
        end;
        line3d(points2[1,Flats[i,4],1],points2[1,Flats[i,4],2],points2[1,Flats[i,4],3],
            points2[1,Flats[i,1],1],points2[1,Flats[i,1],2],points2[1,Flats[i,1],3]);
        line3d(points2[2,Flats[i,4],1],points2[2,Flats[i,4],2],points2[2,Flats[i,4],3],
            points2[2,Flats[i,1],1],points2[2,Flats[i,1],2],points2[2,Flats[i,1],3]);
{      end;}
   end;
   for i:=1 to 8 do line3d(points2[1,i,1],points2[1,i,2],points2[1,i,3],
                           points2[2,i,1],points2[2,i,2],points2[2,i,3]);
   end else begin
     For i:=1 to 6 do begin
          setcolor(x4+1);
          setfillstyle(1,x4+1);
          for o:=1 to 4 do begin
            if (points2[1,flats[i,o],3]<>(-40)) then begin
              quadrant[o].x:=320+trunc(1000*points2[1,flats[i,o],1]/(points2[1,flats[i,o],3]+40));
              quadrant[o].y:=240+trunc(1000*points2[1,flats[i,o],2]/(points2[1,flats[i,o],3]+40));
            end;
          end;
          FillPoly(SizeOf(quadrant) div SizeOf(PointType), quadrant);
     end;
     setcolor(15);
     for i:=1 to 6 do begin
          If Visible(i,szor1)=true then begin
             for n:=1 to 3 do begin
                line3d(points2[1,Flats[i,n],1],points2[1,Flats[i,n],2],points2[1,Flats[i,n],3],
                       points2[1,Flats[i,n+1],1],points2[1,Flats[i,n+1],2],points2[1,Flats[i,n+1],3]);
             end;
             line3d(points2[1,Flats[i,4],1],points2[1,Flats[i,4],2],points2[1,Flats[i,4],3],
                    points2[1,Flats[i,1],1],points2[1,Flats[i,1],2],points2[1,Flats[i,1],3]);
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
      str(score,s);
      setfillstyle(1,0);
      Bar(600,220,639,260);
      OuttextXY(610,230,'Score :');
      Outtextxy(610,250,s);
   end;

Procedure graph_on;
  var
    grDriver,grMode,ErrCode: Integer;
  begin
    grDriver := Detect;
  InitGraph(grDriver, grMode,'c:\utils\tp\bgi');
  ErrCode := GraphResult;
end;


   var A : tabletype;
       ch : char;
       h,m,s0,s,hund : word;
       c:cube;
BEGIN
   Randomize;
   GetTime(h,m,s,hund);
   s0:=h*3600+60*m+s;
   graph_on;
   setcolor(4);
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
             A.Block.draw(-1);
             A.Block.move(1,1);
             if (not A.nogood(1)) then A.Block.move(1,-1);
     a.draw;
          end;
      'a'  : begin
             A.Block.draw(-1);
             A.Block.move(1,-1);
             if (not A.nogood(1)) then A.Block.move(1,1);
     a.draw;
          end;
       'w' : begin
             A.Block.draw(-1);
             A.Block.move(2,1);
             if (not A.nogood(1)) then A.Block.move(2,-1);
     a.draw;
          end;
      's'  : begin
             A.Block.draw(-1);
             A.Block.move(2,-1);
             if (not A.nogood(1)) then A.Block.move(2,1);
     a.draw;
          end;
       'e' : begin
             A.Block.draw(-1);
             A.Block.move(3,1);
             if (not A.nogood(1)) then A.Block.move(3,-1);
     a.draw;
          end;
      'd'  : begin
             A.Block.draw(-1);
             A.Block.move(3,-1);
             if (not A.nogood(1)) then A.Block.move(3,1);
     a.draw;
          end;
      'r'  : begin
             A.Block.draw(-1);
             A.Block.rotate(1,1);
             if (not A.nogood(1)) then A.Block.rotate(1,-1);
     a.draw;
          end;
      'f'  : begin
             A.Block.draw(-1);
             A.Block.rotate(2,1);
             if (not A.nogood(1)) then A.Block.rotate(2,-1);
     a.draw;
          end;
      'v'  : begin
             A.Block.draw(-1);
             A.Block.rotate(3,1);
             if (not A.nogood(1)) then A.Block.rotate(3,-1);
     a.draw;
          end;
      't'  : begin
             A.Block.draw(-1);
             A.Block.rotate(4,1);
             if (not A.nogood(1)) then A.Block.rotate(4,-1);
     a.draw;
          end;
      'g'  : begin
             A.Block.draw(-1);
             A.Block.rotate(5,1);
             if (not A.nogood(1)) then A.Block.rotate(5,-1);
     a.draw;
          end;
      'b'  : begin
             A.Block.draw(-1);
             A.Block.rotate(6,1);
             if (not A.nogood(1)) then A.Block.rotate(6,-1);
             a.draw;
          end;
      'y'  : begin
             A.Block.draw(-1);
             C.rollcamera(1,1);
             if (not A.nogood(1)) then C.rollcamera(1,-1);
             a.draw;
          end;
      'u'  : begin
             A.Block.draw(-1);
             C.rollcamera(1,-1);
             if (not A.nogood(1)) then C.rollcamera(1,1);
             a.draw;
          end;
      'h'  : begin
             A.Block.draw(-1);
             C.rollcamera(2,1);
             if (not A.nogood(1)) then C.rollcamera(2,-1);
             a.draw;
          end;
      'j'  : begin
             A.Block.draw(-1);
             C.rollcamera(2,-1);
             if (not A.nogood(1)) then C.rollcamera(2,1);
             a.draw;
          end;
      'n'  : begin
             A.Block.draw(-1);
             C.rollcamera(3,1);
             if (not A.nogood(1)) then C.rollcamera(3,-1);
             a.draw;
          end;
      'm'  : begin
             A.Block.draw(-1);
             C.rollcamera(3,-1);
             if (not A.nogood(1)) then C.rollcamera(3,1);
             a.draw;
          end;
      ' '  : begin
             repeat
               A.block.draw(-1);
               A.block.down(-1);
               A.block.draw(1);
             until A.leert(1);
          end;
       end;
     end;
     GetTime(h,m,s,hund);
     s:=h*3600+60*m+s;
     if not A.leert(1) then
     if (s-10)>s0 then begin
       A.block.Draw(-1);
       A.block.Down(-1);
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