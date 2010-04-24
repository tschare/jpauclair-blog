package 
{
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.BlendMode;
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.events.TextEvent;
	import flash.geom.Rectangle;
	import flash.text.TextField;
	import flash.utils.ByteArray;
	import flash.utils.getTimer;
	
	/**
	 * ...
	 * @author jpauclair
	 */
	public class Main extends Sprite 
	{
		
		public function Main():void 
		{
			
			if (stage) init();
			else addEventListener(Event.ADDED_TO_STAGE, init);
		}
		
		private function init(e:Event = null):void 
		{
			removeEventListener(Event.ADDED_TO_STAGE, init);
			// entry point
			
			
			addChild(tx);			


			var v1:Vector.<uint> = new Vector.<uint>(1000*1000);
			var v2:Vector.<uint> = new Vector.<uint>(1000*1000);
			for (var i:int = 0; i < 1000*1000; i++ )
			{
				v1[i] = 1;
				v2[i] = 3;
			}
			var t:int = getTimer();
			//v2 = SumVectors(v1, v2);
			v2 = ManualSumVectors(v1, v2);
//			trace(getTimer() - t);

			tx.text = (getTimer() - t).toString();

			return;
			for (i = 0; i < 10; i++)
			{
				trace(v2[i]&0xFFFFFF);
			}
		}
		private var tx:TextField = new TextField();
		
		private const bd1:BitmapData = new BitmapData(1000, 1000, false, 0x0);
		private const bd2:BitmapData = new BitmapData(1000, 1000, false, 0x0);
		private const rect1:Rectangle = bd1.rect;
		
		final private function SumVectors(v1:Vector.<uint>, v2:Vector.<uint>) : Vector.<uint>
		{
			var r:Rectangle = new Rectangle(0, 0, 1000, 1000);
			bd1.setVector(r, v1);
			bd2.setVector(r, v2);
			bd2.draw(bd1 , null, null, BlendMode.ADD);
			v2 = bd2.getVector(r);		
			return v2;
		}
		
		final private function ManualSumVectors(v1:Vector.<uint>, v2:Vector.<uint>) : Vector.<uint>
		{
			var v3:Vector.<uint> = new Vector.<uint>(v1.length);
			for (var i:int = v1.length-1; i >= 0 ; i--)
			{
				v3[i] = v1[i] + v2[i];
			}
			return v3;
		}
		
	}
	
}