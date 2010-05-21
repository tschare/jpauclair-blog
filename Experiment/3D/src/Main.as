package 
{
	import flash.display.GraphicsGradientFill;
	import flash.display.GraphicsPath;
	import flash.display.GraphicsSolidFill;
	import flash.display.GraphicsStroke;
	import flash.display.GraphicsTrianglePath;
	import flash.display.IGraphicsData;
	import flash.display.JointStyle;
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.geom.Matrix;
	import flash.geom.Matrix3D;
	import flash.geom.Vector3D;
	import flash.utils.getTimer;
	
	/**
	 * ...
	 * @author jpauclair
	 */
	public class Main extends Sprite 
	{
		
		public function Main():void 
		{
			//if (stage) init();
			//else addEventListener(Event.ADDED_TO_STAGE, foobar);
			foobar();
		}
		
		private function foobar(e:Event = null):void 
		{
			
			trace("foo");
			var m1:flash.geom.Matrix3D = new flash.geom.Matrix3D();
			var v11:Vector.<Number> = new Vector.<Number>;
			var v2:Vector.<Number> = new Vector.<Number>;
			v11.length = 1000000*3;
			v2.length = 1000000*3;
			
			//var v1:Vector3D = new Vector3D(3, 4, 5);
			var pos:int = 0;
			for (var x1:int = 0; x1 < 1000000; x1++)
			{
				v11[pos++] = Math.random()*100;
				v11[pos++] = Math.random()*100;
				v11[pos++] = Math.random()*100;
			}
			var t:int = getTimer();	
			m1.transformVectors(v11, v2);	
			var t9:int =  getTimer() - t			
			trace(t9);
			
			return;
			
			removeEventListener(Event.ADDED_TO_STAGE, init);
			// entry point
			
			var m:Matrix3D = new Matrix3D();
			m.identity();
			m.appendScale(2, 2, 2);
			
			var v1:Vector.<Number> = new Vector.<Number>;
			v1.push(1, 1, 1, 4, 4, 4);
			
			m.transformVectors(v1, v1);
			trace(v1);
			
			return;
			// stroke object
			var stroke:GraphicsStroke = new GraphicsStroke(3);
			stroke.joints = JointStyle.MITER;
			stroke.fill = new GraphicsSolidFill(0x102020); // solid stroke
			 
			// fill object
			var fill:GraphicsGradientFill = new GraphicsGradientFill();
			fill.colors = [0x0000FF, 0xEEFFEE];
			fill.matrix = new Matrix();
			fill.matrix.createGradientBox(70,70, Math.PI/2);
			 
			// path object
			var path:GraphicsPath = new GraphicsPath(new Vector.<int>(), new Vector.<Number>());
			path.commands.push(1,2,2);
			path.data.push(125,0, 50,100, 175,0);
			
			
			var path2:GraphicsTrianglePath = new GraphicsTrianglePath();
			var tri:Vector.<Number> = new Vector.<Number>();
			path2.vertices = tri;
			path2.vertices.push(0, 100, 100, 100, 200, 0);
			path2.vertices.push(0, 200, 100, 200, 200, 2);

			var fill2:GraphicsGradientFill = new GraphicsGradientFill();
			fill2.colors = [0x00FFFF, 0x00FF00];
			fill2.matrix = new Matrix();
			fill2.matrix.createGradientBox(70,70, Math.PI/2);
			
			
			 
			// combine objects for complete drawing
			var drawing:Vector.<IGraphicsData> = new Vector.<IGraphicsData>();
			drawing.push(stroke, fill, path,fill2,path2);
			//drawing.push(stroke, fill, path2);
			 
			// draw the drawing multiple times
			// change one value to modify each variation
			graphics.drawGraphicsData(drawing);
			path.data[2] += 200;
			
			graphics.drawGraphicsData(drawing);
			path.data[2] -= 150;
			graphics.drawGraphicsData(drawing);
			path.data[2] += 100;
			graphics.drawGraphicsData(drawing);
			path.data[2] -= 50;
			graphics.drawGraphicsData(drawing);			
		}
		
	}
	
}