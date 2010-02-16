package 
{

	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.Sprite;
	import flash.display.Stage;
	import flash.events.Event;
	import flash.events.TimerEvent;
	import flash.filters.GlowFilter;
	import flash.geom.Rectangle;
	import flash.text.TextField;
	import flash.text.TextFieldAutoSize;
	import flash.text.TextFormat;
	import flash.utils.getTimer;
	import flash.utils.Timer;

	import flash.sampler.getMemberNames;
	import flash.system.ApplicationDomain;
	import flash.system.System;
	import flash.utils.getQualifiedClassName;
	
	
	public class FlashStats extends Sprite 
	{
		protected static var BACKGROUND_COLOR:uint = 0xff666666;
		protected static var LINE_COLOR:uint = 0x33999999;
		protected static var GRAPH_COLOR:uint = 0xff93D27C;
		
		private static var EVENT_ADDED_TO_STAGE:String = "addedToStage";
		private static var EVENT_REMOVED_FROM_STAGE:String = "removedFromStage";

		private var _mytext:TextField;
		private var _mytextMainLoop:TextField;
		private var _fps:uint;
		private var _timer:Timer;
		private var _graph:BitmapData;
		private var _lineRect:Rectangle = new Rectangle();
		
		private var _mytextMemory:TextField;
		private var _mytextChilds:TextField;
		private var _lastTotalMem:uint;
		private var _lastTotalChild:uint;
		private var _lastChildAdded:uint;
		private var _lastChildRemoved:uint;		
		private var mDisplaySpriteCount:Boolean = true;
		public var mStage:Stage = null;
		
		
		/**
		* Class Constructor.
		*/
		public function FlashStats() 
		{
			Init();
			this.mouseChildren = false;
			this.mouseEnabled = false;
		}

		
		/**
		 * Init everything
		 */
		private function Init():void 
		{
			var myformat:TextFormat = new TextFormat( "_sans", 11, 0xffffff, false );
			var myglow:GlowFilter = new GlowFilter( 0x333333, 1, 2, 2, 3, 2, false, false );
		
			// fps graph
			var graphHolder:Sprite = new Sprite();
			graphHolder.x = 96;
			addChild( graphHolder );
			_graph = new BitmapData( 40, 12, false, BACKGROUND_COLOR );
			
			var bmp:Bitmap = new Bitmap( _graph )
			bmp.y = 3;
			graphHolder.addChild( bmp );
			DrawGraphLines();

			// display label
			_mytext = new TextField();
			_mytext.autoSize = TextFieldAutoSize.LEFT;
			_mytext.defaultTextFormat = myformat;
			_mytext.selectable = false;
			_mytext.text = "";
			_mytext.filters = [ myglow ];
			_mytext.y = -1;
			addChild( _mytext );

			_mytextMemory = new TextField();
			_mytextMemory.autoSize = TextFieldAutoSize.LEFT;
			_mytextMemory.defaultTextFormat = myformat;
			_mytextMemory.selectable = false;
			_mytextMemory.text = "0 Ko";
			_mytextMemory.filters = [ myglow ];
			_mytextMemory.x = 150;
			_mytextMemory.y = -1;
			addChild( _mytextMemory );						
			
			this.addEventListener(EVENT_ADDED_TO_STAGE, OnSelfAddedToStage);
			
			// setup our timers
			_fps = 0;
			_timer = new Timer( 1000 );
			_timer.addEventListener( TimerEvent.TIMER, OnTimerEvent );
			_timer.start();
			addEventListener( Event.ENTER_FRAME, OnEnterFrame );
	
			

		}

		public function set DisplaySpriteCount(enable:Boolean) : void
		{
			mDisplaySpriteCount = enable;
			this.stage.addEventListener(EVENT_ADDED_TO_STAGE, OnAddedToStage, true);
			this.stage.addEventListener(EVENT_REMOVED_FROM_STAGE, OnRemovedFromStage, true);
		}
		
		public function OnSelfAddedToStage(e:Event) : void
		{
			this.removeEventListener(EVENT_ADDED_TO_STAGE, OnSelfAddedToStage);
			//this.stage.nativeWindow.stage.addEventListener(EVENT_ADDED_TO_STAGE, OnAddedToStage, true);
			this.stage.addEventListener("addedToStage", OnAddedToStage, true);
			this.stage.addEventListener(EVENT_REMOVED_FROM_STAGE, OnRemovedFromStage, true);
			_lastTotalChild = 0;
		}
		public function OnAddedToStage(e:Event) : void
		{
			_lastChildAdded += 1;
			_lastTotalChild += 1;
		}		
		public function OnRemovedFromStage(e:Event) : void
		{
			_lastChildRemoved += 1;
			_lastTotalChild -= 1;
		}				
		/**
		 * Draw a line in the FPS graph
		 */
		private function DrawGraphLines():void 
		{
			for ( var i:Number = 5; i < _graph.height; i += 5 ) 
			{
				_lineRect.x = 0;
				_lineRect.y = i;
				_lineRect.width = _graph.width;
				_lineRect.height = 1;
				//var lineRect:Rectangle = new Rectangle( 0, i, _graph.width, 1 );
				_graph.fillRect( _lineRect, LINE_COLOR );
			}
		}

		/**
		 * Set the screen position of the component
		 * @param	newx
		 * @param	newy
		 */
		public function SetPosition( newx:Number, newy:Number ):void 
		{
			x = newx;
			y = newy;
		}
		
		/**
		 * Manage the onEnterFrame event to count the number of frame
		 * @param	e
		 */
		private var mLastUpdateTime:int = 0;
		private var mLastGCtime:int = 0;
		private function OnEnterFrame( e:Event ):void 
		{
			var totalMem:int = int(System.totalMemory / 1000)
			var diff:int = totalMem - _lastTotalMem;
			var time:int = getTimer();
			
			if (diff<0)
			{
				mLastGCtime = time-mLastUpdateTime;
			}
			mLastUpdateTime = time;
			_fps++;
		}

		/**
		 * Manage the Timer event to update number on screen
		 * @param	e
		 */
		private function OnTimerEvent( e:Event ):void 
		{
			// update our graph for the current tick
			_lineRect.x = _graph.width -1 ;
			_lineRect.y = 0;
			_lineRect.width = 1;
			_lineRect.height = _graph.height;			
			_graph.fillRect( _lineRect, BACKGROUND_COLOR );
			var val:Number = ( _fps / 2 );
			_lineRect.x = _graph.width - 1;
			_lineRect.y = _graph.height - val;
			_lineRect.width = 1;
			_lineRect.height = val;			
			_graph.fillRect( _lineRect, GRAPH_COLOR );
			_graph.scroll( -2, 0 );
			DrawGraphLines();
			// update our label + reset the counter
			_mytext.text = _fps.toString()
			if (stage != null)
			{
				 _mytext.text = "" + _mytext.text +  "  /  " + stage.frameRate + "  FPS ";
			}


			var totalMem:int = int(System.totalMemory / 1000)
			var diff:int = totalMem - _lastTotalMem;
			_lastTotalMem = totalMem;
			
			_mytextMemory.text = "Mem= " + totalMem + " Ko ";
			_mytextMemory.appendText(" ( ");
			if (diff > 0)
			{
				_mytextMemory.appendText("+");
			}
			 _mytextMemory.appendText("" + diff + " Ko ) ");
			
			//if (diff<0)
			{
				_mytextMemory.appendText(" \t\t Last GC= " +  mLastGCtime + " ms ");
			}
				
			if (mDisplaySpriteCount)
			{				
				_mytextMemory.appendText(" \t\t Sprite Count: ");
				_mytextMemory.appendText("" + _lastTotalChild);
				_mytextMemory.appendText(" (+" + _lastChildAdded + " / -" + _lastChildRemoved + ")");
				_mytextMemory.appendText("  :  (+" + Math.round(_lastChildAdded/_fps) + " / -" + Math.round(_lastChildRemoved/_fps) + ")pf");
				_lastChildAdded = 0;
				_lastChildRemoved = 0;
			}
			
			_fps = 0;
		}
	}
}

//Singleton-Blocker : Prevent the instanciation of the FpsDisplay
internal class SingletonBlocker 
{ }
