package  
{
	import flash.display.GradientType;
	import flash.display.Shape;
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
		private static const COLOR_ALPHA:Number = 0.25;
		private static const COLOR_BACKGROUND:int = 0x444444;
		
		private var mMainSprite:Sprite = null;
		private var mAssetsList:Vector.<DisplayObject> = new Vector.<DisplayObject>();
		private var mMaxSize:int = 0;
		private var mAssetsDict:Dictionary = new Dictionary(true);		
		
		private var renderTarget1:Shape = new Shape();
		private var renderTarget2:Shape = new Shape();
		private var currentRenderTarget:Shape = renderTarget2;
		
		public function UsageOverlay(mainSprite:Sprite) 
		{
			mMainSprite = mainSprite;
			Init();
		}
		
		private function Init() : void
		{
			this.addChild(renderTarget1);
			this.addChild(renderTarget2);
			this.mouseChildren = false;
			this.mouseEnabled = false;
			mMainSprite.stage.addEventListener(Event.ADDED_TO_STAGE, OnAddedToStage, true);
			mMainSprite.stage.addEventListener(Event.REMOVED_FROM_STAGE, OnRemovedToStage, true);
			mMainSprite.stage.addEventListener(Event.ENTER_FRAME, Update);
			trace("Usage Overlay initialized");
			this.swapChildren(renderTarget1, renderTarget2);
		}
		
		private function SwapRenderTarget() : void
		{
			if (currentRenderTarget == renderTarget1)
			{
				currentRenderTarget = renderTarget2;
			}
			else
			{
				currentRenderTarget = renderTarget1;
			}
			this.swapChildren(renderTarget1, renderTarget2);
		}
		private function Update(e:Event):void 
		{
			SwapRenderTarget();
		
			
			currentRenderTarget.graphics.clear();

			currentRenderTarget.graphics.beginFill(COLOR_BACKGROUND, COLOR_ALPHA/30);
			currentRenderTarget.graphics.drawRect(0,0,mMainSprite.stage.stageWidth,mMainSprite.stage.stageHeight);
			currentRenderTarget.graphics.endFill();
			
			var rect:Rectangle = null;
			//return; 
			/*
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

						currentRenderTarget.graphics.beginFill(color, ratio/2/0xff);
						currentRenderTarget.graphics.drawRect(rect.x, rect.y, rect.width, rect.height);
						currentRenderTarget.graphics.endFill();

					}
				}
				i--;
			}
			
			*/
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
					currentRenderTarget.graphics.beginFill(COLOR_BLUE, COLOR_ALPHA/2);
					currentRenderTarget.graphics.drawRect(rect.x, rect.y, rect.width, rect.height);
					currentRenderTarget.graphics.endFill();
					
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
					currentRenderTarget.graphics.beginFill(COLOR_RED, 0.9);
					currentRenderTarget.graphics.drawRect(rect.x, rect.y, rect.width, rect.height);
					currentRenderTarget.graphics.endFill();

					mAssetsDict[obj] = true;
				}
				else
				{
					currentRenderTarget.graphics.beginFill(COLOR_YELLOW, 1);
					currentRenderTarget.graphics.drawRect(rect.x-15, rect.y, rect.width, rect.height);
					currentRenderTarget.graphics.endFill();
				}
			}
		}

		private function OnRemovedToStage(e:Event):void 
		{
			var obj : DisplayObject = e.target as DisplayObject;
			
			if (obj != mMainSprite)
			{
				
				var rect:Rectangle = obj.getRect(mMainSprite);

				currentRenderTarget.graphics.beginFill(COLOR_GREEN, 0.9);
				currentRenderTarget.graphics.drawRect(rect.x-2, rect.y-2, rect.width+4, rect.height+4);
				currentRenderTarget.graphics.endFill();
				
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