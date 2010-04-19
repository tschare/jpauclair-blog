package  
{
	import flash.display.GradientType;
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.geom.Rectangle;
	import flash.display.DisplayObject;
	import flash.display.DisplayObjectContainer;
	import flash.sampler.getSize;
	import flash.utils.Dictionary;
	/**
	 * ...
	 * @author jpauclair
	 */
	public class UsageOverlay extends Sprite
	{
		private static const COLOR_RED:int =	0x9C0804;
		private static const COLOR_YELLOW:int =	0xFFA929;
		private static const COLOR_GREEN:int =	0x85FF27;
		private static const COLOR_BLUE:int =	0x34D8E8;
		private static const COLOR_PINK:int =	0xFF0DFF;
		private static const COLOR_ALPHA:Number = 0.5;
		private static const COLOR_BACKGROUND:int = 0x444444;
		
		private var mMainSprite:Sprite = null;
		private var mAssetsList:Vector.<DisplayObject> = new Vector.<DisplayObject>();
		private var mMaxSize:int = 0;
		private var mAssetsDict:Dictionary = new Dictionary(true);		
		
		public function UsageOverlay(mainSprite:Sprite) 
		{
			mMainSprite = mainSprite;
			Init();
		}
		
		private function Init() : void
		{
			this.mouseChildren = false;
			this.mouseEnabled = false;
			mMainSprite.stage.addEventListener(Event.ADDED_TO_STAGE, OnAddedToStage, true);
			mMainSprite.stage.addEventListener(Event.REMOVED_FROM_STAGE, OnRemovedToStage, true);
			mMainSprite.stage.addEventListener(Event.ENTER_FRAME, Update);
			trace("Usage Overlay initialized");
		}
		
		private function Update(e:Event):void 
		{
			this.graphics.clear();

			this.graphics.beginFill(COLOR_BACKGROUND, COLOR_ALPHA);
			this.graphics.drawRect(0,0,mMainSprite.stage.stageWidth,mMainSprite.stage.stageHeight);
			this.graphics.endFill();
			
			var rect:Rectangle = null;
			//return; 
			
			mAssetsList.splice(0, mAssetsList.length);
			mMaxSize = 0;
			//MapMemorySize(MainSprite);

			
			var i:int = mAssetsList.length-1;
			
			while (i >= 0)
			{
				var obj:DisplayObject = mAssetsList[i];
				if (obj != null)
				{
					rect = obj.getRect(mMainSprite);
			
					var ratio:Number = getSize(obj) / mMaxSize * 0xFF;
					if (ratio < 0xFF)
					{
						//var ratio:Number = getSize(obj) / 1000 * 0xFF;
						var color:uint = ratio << 8;
						color += (0xFF-ratio);

						this.graphics.beginFill(color, ratio/2/0xff);
						this.graphics.drawRect(rect.x, rect.y, rect.width, rect.height);
						this.graphics.endFill();

					}
				}
				i--;
			}
			
			//return;
			
			var count:int = 0;
			var count1Size:int = 0;
			for (var obj3:Object in mAssetsDict)
			{
				if (obj3.stage != null && mAssetsDict[obj3] == false)
				{
					rect = obj3.getRect(mMainSprite);
					//count1Size += getSize(obj3);
					count++;
//					trace(rect);	
					this.graphics.beginFill(COLOR_BLUE, COLOR_ALPHA/2);
					this.graphics.drawRect(rect.x, rect.y, rect.width, rect.height);
					this.graphics.endFill();
					
				}
			}
		}
		
		private function OnAddedToStage(e:Event):void 
		{
			var obj : DisplayObject = e.target as DisplayObject;
			
			if (obj != mMainSprite)
			{
				var rect:Rectangle = obj.getRect(mMainSprite);
				var newObj:Boolean = true;
				if (mAssetsDict[obj] == true)
				{
					newObj = false;
				}
				if (newObj)
				{
					this.graphics.beginFill(COLOR_RED, COLOR_ALPHA);
					/*
					var colors:Array = new Array(COLOR_RED);// , 0x0);
					var alphas:Array = new Array(COLOR_ALPHA, 0);
					var ratios:Array = [0x00, 0xFF];
					this.graphics.beginGradientFill(GradientType.LINEAR, colors, alphas, ratios);
					*/
					this.graphics.drawRect(rect.x, rect.y, rect.width, rect.height);
					this.graphics.endFill();

					mAssetsDict[obj] = true;
				}
				else
				{
					this.graphics.beginFill(COLOR_YELLOW, COLOR_ALPHA);
					this.graphics.drawRect(rect.x, rect.y, rect.width, rect.height);
					this.graphics.endFill();
				}
			}
		}

		private function OnRemovedToStage(e:Event):void 
		{
			var obj : DisplayObject = e.target as DisplayObject;
			if (obj != mMainSprite)
			{
				var rect:Rectangle = obj.getRect(mMainSprite);

				this.graphics.beginFill(COLOR_GREEN, COLOR_ALPHA);
				this.graphics.drawRect(rect.x, rect.y, rect.width, rect.height);
				this.graphics.endFill();
				
				mAssetsDict[obj] = false;
			}
		}
		
		private function MapMemorySize(obj:DisplayObject) : void
		{
			var i:int = 0;
			var container:DisplayObjectContainer = obj as DisplayObjectContainer;
			if (container != null)
			{
				
				for (i = container.numChildren-1; i >= 0; i--)
				{
					MapMemorySize(container.getChildAt(i));
				}
			}
			else
			{

				
			}
				//if (obj != MainSprite)
				{
					mAssetsList.push(obj);
					var size:int = getSize(obj)
					if (size > mMaxSize) mMaxSize = size;
				}			
		}
		
	}
}