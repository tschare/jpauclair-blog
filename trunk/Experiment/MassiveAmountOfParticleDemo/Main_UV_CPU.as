package 
{
	// Flash Molehill : Massive amounts of  3d particles with GPU 
	// Jean-Philippe Auclair (jpauclair)
	// jpauclair.net
	
	// PBender -> http://www.unitzeroone.com/blog/2009/03/18/flash-10-massive-amounts-of-3d-particles-with-alchemy-source-included/
	// AS3     -> http://blog.joa-ebert.com/2009/04/03/massive-amounts-of-3d-particles-without-alchemy-and-pixelbender/
	// haXe    -> http://webr3.org/blog/haxe/flash-10-massive-amounts-of-3d-particles-with-haxe/
	
	import flash.display.StageScaleMode;
	import flash.filters.GlowFilter;
	import flash.system.ApplicationDomain;
	import flash.system.Capabilities;
	import flash.system.System;
	import flash.text.TextField;
	import flash.text.TextFieldAutoSize;
	import flash.text.TextFormat;
	import flash.display.Sprite;
	import flash.display.Stage3D;
	import flash.display3D.Context3D;
	import flash.display3D.Context3DBlendFactor;
	import flash.display3D.Context3DCompareMode;
	import flash.display3D.Context3DProgramType;
	import flash.display3D.Context3DRenderMode;
	import flash.display3D.Context3DTextureFormat;
	import flash.display3D.Context3DTriangleFace;
	import flash.display3D.Context3DVertexBufferFormat;
	import flash.display3D.IndexBuffer3D;
	import flash.display3D.Program3D;
	import flash.display3D.textures.Texture;
	import flash.display3D.VertexBuffer3D;
	import flash.events.Event;
	import flash.events.KeyboardEvent;
	import flash.events.MouseEvent;
	import flash.events.TimerEvent;
	import flash.geom.Matrix3D;
	import flash.geom.Point;
	import flash.geom.Rectangle;
	import flash.geom.Vector3D;
	import flash.ui.Keyboard;
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	import flash.utils.Timer;

	//Fast memory (Azoth post-processor)
	import com.adobe.utils.AGALMiniAssembler;
	import com.buraks.utils.fastmem;

	[SWF(width = "800", height = "600", frameRate = "60", backgroundColor = "#0")]
	public class Main_UV_CPU extends Sprite 
	{
		public var _renderTimer:Timer;
		
		private const MAX_PARTICLES: uint = (Math.pow(2,16))/4 -1; //Limited to 16 bits (2^16) (64535) (with 4 vertice per quad) = 16K particles
		
		private var _context:Context3D;
		private var _projectionMatrix:Matrix3D;
		private var _viewMatrix:Matrix3D;
		private var _invertViewMatrix:Matrix3D = new Matrix3D();
		
		private var _numCol:int = 16;
		private var _numRow:int = 5;
		
		private var _currentParticleFrame:Vector.<int> = new Vector.<int>(MAX_PARTICLES, true);
		
		private var _singleFrameUVWidth:Number = 32/512;
		private var _singleFrameUVHeight:Number = 32/512;			
		
		private var _animFrameCount:Number = 71;// 8 * 8;			
		
		private var _vertexBufferData:ByteArray = null;
		private var _vertexBuffer:VertexBuffer3D= null;
		
		
		private var _indexBufferData:ByteArray = null;
		private var _indexBuffer:IndexBuffer3D = null;
		private var _texture:Texture = null;
		private var _textureMask:Texture = null;
		
		private var _fragmentConsts:Vector.<Number> = Vector.<Number>([ 0, 0, 1, 0.05]);

		private var _Infos:TextField;		
		
		
		[Embed(source = '../lib/space-core_black.atf',mimeType="application/octet-stream")]
		private var _SpriteSheetATF:Class;		
		
		[Embed(source = '../lib/space-core_mask.atf',mimeType="application/octet-stream")]
		private var _SpriteSheetMaskATF:Class;		
		
		private var _UseIncubator:Boolean = false;
		
		public function Main_UV_CPU():void 
		{
			if (stage) init();
			else addEventListener(Event.ADDED_TO_STAGE, init);
		}
		
		private function init(e:Event = null):void 
		{
			removeEventListener(Event.ADDED_TO_STAGE, init);
			// entry point
			
			// timer for animation
			_renderTimer = new Timer(1);
			_renderTimer.addEventListener(TimerEvent.TIMER, HandleTimer);
			_renderTimer.start();	
			
			var myformat:TextFormat = new TextFormat( "_sans", 11, 0xffffffff, false );
			var myglow:GlowFilter = new GlowFilter( 0xff333333, 1, 2, 2, 3, 2, false, false );
			
			
			_Infos= new TextField();
			_Infos.mouseEnabled = false;
			_Infos.autoSize = TextFieldAutoSize.LEFT;
			_Infos.defaultTextFormat = myformat;
			_Infos.selectable = false;
			_Infos.filters = [ myglow ];
			_Infos.appendText("Press 1, 2, 3, 4 to change the number of processed Attractors, 5 to change size\n");
			_Infos.appendText("Each attractor have 16K particles\n");
			_Infos.appendText("Each particle is a SpriteSheet animation of 71 frames using transparency mask");
			_Infos.x = 10;
			_Infos.y = stage.stageHeight - _Infos.textHeight-10;
			addChild(_Infos);

			SetupContext();
		}
		
		private function SetupContext() : void
		{
			trace(Capabilities.version)
			
			var a:Object;
			
			try {
				this.stage.stage3Ds[0]["viewPort"] = new Rectangle(0, 0, stage.stageWidth, stage.stageHeight);
				_UseIncubator = false;
			}
			catch (e:Error)
			{
				_UseIncubator = true;
			}

			this.stage.stage3Ds[0].addEventListener ( "context3DCreate", OnContext );
			this.stage.stage3Ds[0].requestContext3D(Context3DRenderMode.AUTO)	
		}
		
		private function OnContext(e:Event):void 
		{
			var t : Stage3D = e.target as Stage3D;
			_context = t.context3D; 	
			
			if (_UseIncubator )
			{
				this.stage.stage3Ds[0]["x"] = 0
				this.stage.stage3Ds[0]["y"] = 0
			}
			
			this.stage.scaleMode = StageScaleMode.NO_SCALE
			_context.configureBackBuffer(this.stage.stageWidth, this.stage.stageHeight,8,true); // fixed size
			_context.setCulling(Context3DTriangleFace.BACK);
			_context.setDepthTest(true, Context3DCompareMode.LESS); 
			_context.setBlendFactors( Context3DBlendFactor.SOURCE_ALPHA, Context3DBlendFactor.ONE_MINUS_SOURCE_ALPHA );				
			
			CreateParticles();
			
			InitStreams();
			
			InitProgram();
			
			InitProgramConstants();
			
			InitTextures(); 
			
			InitCamera();
			
			
			addChild(focusManager);
			focusManager.addEventListener(KeyboardEvent.KEY_UP, OnKeyUp)
		}
		private var focusManager:Sprite = new Sprite();
		private function OnMouseClick(e:MouseEvent):void 
		{
			mNumberToShow++;
			if (mNumberToShow > 4) mNumberToShow = 1;
		}
		
		private function OnKeyUp(e:KeyboardEvent):void 
		{
			if (e.charCode == Keyboard.NUMBER_1) mNumberToShow = 1;
			if (e.charCode == Keyboard.NUMBER_2) mNumberToShow = 2;
			if (e.charCode == Keyboard.NUMBER_3) mNumberToShow = 3;
			if (e.charCode == Keyboard.NUMBER_4) mNumberToShow = 4;
			
			if (e.charCode == Keyboard.NUMBER_5)
			{
				var quadSize:Number = 0.075;
				var vertexOffset:Vector.<Number> = null;
				
				
				
				_sizeToUse = _sizeToUse % 5
				switch (_sizeToUse)
				{
					case 0: quadSize = 0.075; break;
					case 1: quadSize = 0.05; break;
					case 2: quadSize = 0.025; break;
					case 3: quadSize = 0.0125; break;
					case 4: quadSize = 0.1; break;
					
				}
				_sizeToUse++;
				vertexOffset = Vector.<Number>([-quadSize, -quadSize, 0, 1]);
				_context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, 12,vertexOffset, 1);	
				
				vertexOffset = Vector.<Number>([quadSize, -quadSize, 0, 1]);
				_context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, 13,vertexOffset, 1);	
				
				vertexOffset = Vector.<Number>([ -quadSize, quadSize, 0, 1]);
				_context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, 14,vertexOffset, 1);	
				
				vertexOffset = Vector.<Number>([quadSize, quadSize, 0, 1]);
				_context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, 15,vertexOffset, 1);					
				
				
			}
			stage.focus = focusManager
		}
		
		public function CreateParticles() : void
		{
			var positions:Vector.<Vector3D> = new Vector.<Vector3D>(MAX_PARTICLES * 4,true);
			var indexes:Vector.<uint> = new Vector.<uint>(MAX_PARTICLES * 6,true);
			var uvs:Vector.<Point> = new Vector.<Point>(MAX_PARTICLES * 4, true);

			var vertexPositionIdx:int = 0;
			var vertexIndexIdx:int = 0;
			var vertexUVIdx:int = 0;

			var _a:Number = 1.111;
			var _b:Number = 1.479;
			var _f:Number = 4.494;
			var _g:Number = 0.44;
		 	var _d:Number = 0.135;
			var cx:Number = 1;
			var cy:Number = 1;
			var cz:Number = 1;
			var mx:Number = 0;
			var my:Number = 0;
			var mz:Number = 0;
			
			var scale:Number = 0.55;// 40;
			var it:int = 0;

			var index:int = 0;
			var RandomDiv:Number = 100;
			while(it <  MAX_PARTICLES )
			{
				mx = cx + _d * (-_a * cx - cy * cy - cz * cz + _a * _f);
				my = cy + _d * (-cy + cx * cy - _b * cx * cz + _g);
				mz = cz + _d * (-cz + _b * cx * cy + cx * cz);
				
				cx = mx;
				cy = my;
				cz = mz;
				
				var x:Number = mx * scale;
				var y:Number = my * scale;
				var z:Number = mz * scale;

				positions[vertexPositionIdx++] = new Vector3D( x, y, z, 12);
				positions[vertexPositionIdx++] = new Vector3D( x, y, z, 13);
				positions[vertexPositionIdx++] = new Vector3D( x, y, z, 14);
				positions[vertexPositionIdx++] = new Vector3D( x, y, z, 15);
				
				indexes[vertexIndexIdx++] = 0 + it * 4;
				indexes[vertexIndexIdx++] = 1 + it * 4;
				indexes[vertexIndexIdx++] = 2 + it * 4;
				
				indexes[vertexIndexIdx++] = 2 + it * 4;
				indexes[vertexIndexIdx++] = 1 + it * 4;
				indexes[vertexIndexIdx++] = 3 + it * 4;
				
				uvs[vertexUVIdx++] = new Point(0, 0);
				uvs[vertexUVIdx++] = new Point(_singleFrameUVWidth, 0);
				uvs[vertexUVIdx++] = new Point(0, _singleFrameUVHeight);
				uvs[vertexUVIdx++] = new Point(_singleFrameUVWidth, _singleFrameUVHeight);
				
				_currentParticleFrame[it] = Math.random() * 71;
				
				it++;
			}
			
			SetFastMem(positions, indexes, uvs);	
			
			fastmem.fastSelectMem(_vertexBufferData);
		}
		
		public function InitStreams() : void
		{
			_context.setVertexBufferAt( 0, _vertexBuffer, 0, Context3DVertexBufferFormat.FLOAT_4  ); // XYZW
			_context.setVertexBufferAt( 1, _vertexBuffer, 4, Context3DVertexBufferFormat.FLOAT_2  ); // UV			
		}
		
		public function InitProgramConstants() : void
		{
			var quadSizeX:Number = 0.075;
			var quadSizeY:Number = 0.075;
			
			_context.setProgramConstantsFromVector( Context3DProgramType.FRAGMENT, 0,_fragmentConsts, 1 );
			
			var vertexOffset:Vector.<Number> = null;
			
			vertexOffset = Vector.<Number>([-quadSizeX, -quadSizeY, 0, 1]);
			_context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, 12,vertexOffset, 1);	
			
			vertexOffset = Vector.<Number>([quadSizeX, -quadSizeY, 0, 1]);
			_context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, 13,vertexOffset, 1);	
			
			vertexOffset = Vector.<Number>([ -quadSizeX, quadSizeY, 0, 1]);
			_context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, 14,vertexOffset, 1);	
			
			vertexOffset = Vector.<Number>([quadSizeX, quadSizeY, 0, 1]);
			_context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, 15,vertexOffset, 1);					
			
		}
		
		public function InitTextures() : void
		{
			var img:ByteArray = new _SpriteSheetATF() as ByteArray;
			_texture = _context.createTexture(512, 512, Context3DTextureFormat.COMPRESSED, false );
			_texture.uploadCompressedTextureFromByteArray(img,0);
			_context.setTextureAt(0, _texture);			
			
			var img2:ByteArray = new _SpriteSheetMaskATF() as ByteArray;
			_texture = _context.createTexture(512, 512, Context3DTextureFormat.COMPRESSED, false );
			_texture.uploadCompressedTextureFromByteArray(img,0);
			_context.setTextureAt(1, _texture);				
		}
		
		public function InitCamera():void
		{
			_viewMatrix = new Matrix3D();
			_projectionMatrix = makeProjectionMatrix();
		}
		
		public function SetFastMem(positions:Vector.<Vector3D>, indexes:Vector.<uint>, uvs:Vector.<Point>) : void
		{
			var tempAllocation:ByteArray = new ByteArray();
			tempAllocation.length = 1024 * 1024*3;
			tempAllocation.endian = Endian.LITTLE_ENDIAN;	
			fastmem.fastSelectMem(tempAllocation);
				
			var len:int = indexes.length
			var i:int = 0;

			for (i = 0; i < len; i++)
			{
				fastmem.fastSetI16(indexes[i], i * 2);
			}
			
			tempAllocation.position = 0;
			
			_indexBufferData = new ByteArray();
			_indexBufferData.endian = Endian.LITTLE_ENDIAN;
			_indexBufferData.length = len * 2;
			_indexBufferData.writeBytes(tempAllocation,0, _indexBufferData.length);			
			
			_indexBuffer = _context.createIndexBuffer(len);
			_indexBuffer.uploadFromByteArray(_indexBufferData, 0, 0, len);
			
			var dwordPerVertice:int = 6;
			
			var vertexCount:int = positions.length;
			
			var pos:Vector3D = null;
			var uv:Point = null;
			var outPos:int = 0;
			
			for (i = 0; i < vertexCount; i++)
			{
				// X Y Z
				pos = positions[i];
				fastmem.fastSetFloat(pos.x,  outPos); outPos += 4; // X
				fastmem.fastSetFloat(pos.y , outPos); outPos += 4; // Y
				fastmem.fastSetFloat(pos.z , outPos); outPos += 4; // Z
				fastmem.fastSetFloat(pos.w , outPos); outPos += 4; // w

				// U V
				uv = uvs[i];
				fastmem.fastSetFloat(uv.x , outPos); outPos += 4; // U
				fastmem.fastSetFloat(uv.y , outPos); outPos += 4; // V
			}
			

			tempAllocation.position = 0;
			
			_vertexBufferData = new ByteArray();
			_vertexBufferData.endian = Endian.LITTLE_ENDIAN;
			_vertexBufferData.length = vertexCount * (4 * dwordPerVertice);
			_vertexBufferData.writeBytes(tempAllocation, 0, _vertexBufferData.length);
			
			_vertexBuffer = _context.createVertexBuffer(vertexCount, dwordPerVertice);
			_vertexBuffer.uploadFromByteArray(_vertexBufferData, 0, 0, vertexCount);

		}

		private function HandleTimer (e:TimerEvent) : void {
			OnEnterFrame ( null);		
		}
		
		private var viewRotationSpeed:Number = 5.1;
		private var viewCurrentRotation1:Number = 0.1;
		private var viewCurrentRotation2:Number = 10.1;
		private var viewCurrentRotation3:Number = 25.1;
		private var viewCurrentRotation4:Number = 40.1;
		
		private var mNumberToShow:int = 1;
		private var _sizeToUse:int = 1;
		private function OnEnterFrame(e:Event):void 
		{
			_context.clear(0, 0, 0, 0);
			
			var spacer:Number = 2;
			var distZ:Number = 8;
			
			switch (mNumberToShow)
			{
				case 1:
					spacer = 0;
					distZ = 5;
					break;
				case 2:
					spacer = 1.3;
					distZ = 7;
					break;
				case 3:
					spacer = 2;
					distZ = 9;
					break;
				case 4:
					spacer = 2;
					distZ = 9;
					break;					
			}
			
			stage.focus = focusManager
			
			if (mNumberToShow >= 1)
			{
				viewCurrentRotation1 += viewRotationSpeed;
				UpdateUV();
				_viewMatrix.identity();
				_viewMatrix.appendRotation(viewCurrentRotation1/ 53,	 Vector3D.Z_AXIS);
				_viewMatrix.appendRotation(viewCurrentRotation1/ 29,	 Vector3D.Y_AXIS);
				_viewMatrix.appendRotation(viewCurrentRotation1 / 71, Vector3D.X_AXIS); // the classic prime spin
				_viewMatrix.copyToMatrix3D(_invertViewMatrix);
				_viewMatrix.appendTranslation(spacer*4/3, spacer, -distZ); // the classic prime spin
				_viewMatrix.append(_projectionMatrix);
				_invertViewMatrix.invert();
				
				_context.setProgramConstantsFromMatrix( Context3DProgramType.VERTEX, 0, _viewMatrix,true );
				_context.setProgramConstantsFromMatrix( Context3DProgramType.VERTEX, 8, _invertViewMatrix,true );
				
				_context.drawTriangles( _indexBuffer, 0, MAX_PARTICLES*2);
			}
			
			
			if (mNumberToShow >= 2)
			{			
				viewCurrentRotation2 += viewRotationSpeed;
				UpdateUV();
				// the classic prime spin
				_viewMatrix.identity();
				_viewMatrix.appendRotation(viewCurrentRotation2 / 11, Vector3D.Z_AXIS);
				_viewMatrix.appendRotation(viewCurrentRotation2 / 17, Vector3D.Y_AXIS);
				_viewMatrix.appendRotation(viewCurrentRotation2 / 23, Vector3D.X_AXIS); 
				
				_viewMatrix.copyToMatrix3D(_invertViewMatrix);
				
				_viewMatrix.appendTranslation(-spacer*4/3, -spacer, -distZ);
				_viewMatrix.append(_projectionMatrix);
				
				_invertViewMatrix.invert();
				
				_context.setProgramConstantsFromMatrix( Context3DProgramType.VERTEX, 0, _viewMatrix,true );
				_context.setProgramConstantsFromMatrix( Context3DProgramType.VERTEX, 8, _invertViewMatrix, true );
				
				_context.drawTriangles( _indexBuffer, 0, MAX_PARTICLES * 2);
			}
			
			if (mNumberToShow >= 3)
			{			
				viewCurrentRotation3 += viewRotationSpeed;
				UpdateUV();
				_viewMatrix.identity();
				_viewMatrix.appendRotation(viewCurrentRotation3/ 20,	 Vector3D.Z_AXIS);
				_viewMatrix.appendRotation(viewCurrentRotation3/ 30,	 Vector3D.Y_AXIS);
				_viewMatrix.appendRotation(viewCurrentRotation3 / 50, Vector3D.X_AXIS); // the classic prime spin
				
				_viewMatrix.copyToMatrix3D(_invertViewMatrix);
				
				_viewMatrix.appendTranslation(spacer*4/3, -spacer, -distZ); // the classic prime spin
				_viewMatrix.append(_projectionMatrix);
				
				_invertViewMatrix.invert();
				
				_context.setProgramConstantsFromMatrix( Context3DProgramType.VERTEX, 0, _viewMatrix,true );
				_context.setProgramConstantsFromMatrix( Context3DProgramType.VERTEX, 8, _invertViewMatrix, true );
				
				_context.drawTriangles( _indexBuffer, 0, MAX_PARTICLES * 2);
			}
			
			if (mNumberToShow >= 4)
			{
				viewCurrentRotation4 += viewRotationSpeed;
				UpdateUV();
				_viewMatrix.identity();
				_viewMatrix.appendRotation(viewCurrentRotation4/ 150,	 Vector3D.Z_AXIS);
				_viewMatrix.appendRotation(viewCurrentRotation4/ 130,	 Vector3D.Y_AXIS);
				_viewMatrix.appendRotation(viewCurrentRotation4 / 70, Vector3D.X_AXIS); // the classic prime spin
				
				_viewMatrix.copyToMatrix3D(_invertViewMatrix);
				
				_viewMatrix.appendTranslation(-spacer*4/3, spacer, -distZ); // the classic prime spin
				_viewMatrix.append(_projectionMatrix);
				
				_invertViewMatrix.invert();
				
				_context.setProgramConstantsFromMatrix( Context3DProgramType.VERTEX, 0, _viewMatrix,true );
				_context.setProgramConstantsFromMatrix( Context3DProgramType.VERTEX, 8, _invertViewMatrix, true );
				
				_context.drawTriangles( _indexBuffer, 0, MAX_PARTICLES * 2);			
			}
			_context.present();
		}
		
		
		
		private function UpdateUV() : void
		{
			var offsetVertexBuffer:int = 0;
			var vertexDescriptionSize:int = 24;
			
			for (var i:int = 0; i < MAX_PARTICLES; i++)
			{
				var frameIdx:int = _currentParticleFrame[i]++;
				if (frameIdx >= _animFrameCount) { _currentParticleFrame[i] = 0; frameIdx = 0; } 

				var posX:int = int(frameIdx % _numCol); 
				var posY:int = int(frameIdx / _numCol); 
				var newPosX:Number = posX * _singleFrameUVWidth;
				var newPosY:Number = posY * _singleFrameUVHeight;
				
				var localOffsetPos:int = localOffsetPos = offsetVertexBuffer + 16;
				
				//UV vertex 1
				fastmem.fastSetFloat( newPosX, localOffsetPos);
				fastmem.fastSetFloat( newPosY, localOffsetPos + 4);
				localOffsetPos += vertexDescriptionSize;
				
				//UV vertex 2
				fastmem.fastSetFloat(_singleFrameUVWidth + newPosX, localOffsetPos); 
				fastmem.fastSetFloat( newPosY, localOffsetPos + 4); 
				localOffsetPos += vertexDescriptionSize;
				
				//UV vertex 3
				fastmem.fastSetFloat( newPosX, localOffsetPos); 
				fastmem.fastSetFloat(_singleFrameUVHeight + newPosY, localOffsetPos + 4); 
				localOffsetPos += vertexDescriptionSize;
				
				//UV vertex 4
				fastmem.fastSetFloat(_singleFrameUVWidth + newPosX, localOffsetPos); 
				fastmem.fastSetFloat(_singleFrameUVHeight+newPosY, localOffsetPos + 4);
				
				//Jump one quad ahead
				offsetVertexBuffer += vertexDescriptionSize * 4;
			}
			
			_vertexBuffer.uploadFromByteArray(_vertexBufferData, 0, 0, _vertexBufferData.length / vertexDescriptionSize);
		}
		
		private function InitProgram() : void
		{
			var vertexASM:AGALMiniAssembler = new AGALMiniAssembler();

			vertexASM.assemble( Context3DProgramType.VERTEX, 
					"mov vt1, va0 \n" + 
					"mov vt2, vc[va0.w] \n" +  			// new vector (DeltaX,DeltaY,DeltaZ,1)
					"m33 vt3.xyz, vt2.xyz, vc8     \n" +  		// transform InvertView (m33 because it's only a rotation/scale)
					"mov vt3.w, vt2.w \n" + 
					"add vt3.xyz, vt3.xyz, vt1.xyz \n" +// Add the original vertex position
					
					"m44 op, vt3, vc0     \n" +  		// transform ViewProjection matrix
					"mov v1, va1      \n");				// Send UVs to fragment shader
			
			var fragmentASM:AGALMiniAssembler = new AGALMiniAssembler();
			fragmentASM = new AGALMiniAssembler(); 
			fragmentASM.assemble( Context3DProgramType.FRAGMENT,
				"tex ft1, v1, fs0  <2d,linear,miplinear> \n" +  //Diffuse
				"tex ft2, v1, fs1  <2d,linear,miplinear> \n" +  //AlphaMask
				
				"sub ft0.x ,ft2.x,fc0.w \n" + //Substract transparency treshold
				"kil ft0.x \n" + //Kill alpha pixels
				
				"mov oc, ft1"  );	
				
			var pr:Program3D = _context.createProgram();
			
			pr.upload(vertexASM.agalcode, fragmentASM.agalcode);	
			
			_context.setProgram(pr);
		}
		
		public static const DEG2RAD:Number	= Math.PI / 360.0;
		
		public function makeProjectionMatrix ( zNear:Number = 0.1, 
													  zFar:Number = 1000, 
													  fovDegrees:Number = 60, 
													  aspect:Number = 4/3 ):Matrix3D
		{  			
			var yval:Number = zNear * Math.tan( fovDegrees * DEG2RAD );
			var xval:Number = yval * aspect;	
			return makeFrustumMatrix( -xval, xval, -yval, yval, zNear, zFar );
		}				
		
		public function makeFrustumMatrix( left:Number, right:Number, top:Number, bottom:Number, zNear:Number, zFar:Number ):Matrix3D 
		{
			return new Matrix3D(
				Vector.<Number>(
					[
						(2*zNear)/(right-left),
						0,
						(right+left)/(right-left),
						0,
						
						0,
						(2*zNear)/(top-bottom),
						(top+bottom)/(top-bottom),
						0,
						
						0,
						0,
						zFar/(zNear-zFar),
						-1,
						
						0,
						0,
						(zNear*zFar)/(zNear-zFar),
						0
					]
				)
			);
		}		
		
	}
	
}