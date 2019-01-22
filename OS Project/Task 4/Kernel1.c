code Kernel

  -- <ROHAN PATEL>


-----------------------------  ThreadManager  ---------------------------------

  behavior ThreadManager

      ----------  ThreadManager . Init  ----------

      method Init ()
        --
        -- This method is called once at kernel startup time to initialize
        -- the one and only "ThreadManager" object.
        -- 
          var
            i : int
          
          print ("Initializing Thread Manager...\n")

          threadTable = new array of Thread {  MAX_NUMBER_OF_PROCESSES of new Thread}     -- create Array for threads
          freeList = new List [ Thread ]                                                  -- create list for free thredas 
            

          -- initialization of threads
          threadTable[0].Init ("T-1")
          threadTable[1].Init ("T-2")
          threadTable[2].Init ("T-3")
          threadTable[3].Init ("T-4")
          threadTable[4].Init ("T-5")
          threadTable[5].Init ("T-6")
          threadTable[6].Init ("T-7")
          threadTable[7].Init ("T-8")
          threadTable[8].Init ("T-9")
          threadTable[9].Init ("T-10")


          for i = 0 to MAX_NUMBER_OF_PROCESSES - 1
            threadTable[i].status = UNUSED          -- set each thread a status : UNUSED
            freeList.AddToEnd (&threadTable[i])     -- Add each thread to freeList
          endFor  

          -- MutexLock and CV initialization
          threadManagerLock = new Mutex
          threadManagerLock.Init()
          aThreadBecomeFree = new Condition
          aThreadBecomeFree.Init()


      endMethod

      ----------  ThreadManager . Print  ----------

      method Print ()
        -- 
        -- Print each thread.  Since we look at the freeList, this
        -- routine disables interrupts so the printout will be a
        -- consistent snapshot of things.
        -- 
        var i, oldStatus: int
          oldStatus = SetInterruptsTo (DISABLED)
          print ("Here is the thread table...\n")
          for i = 0 to MAX_NUMBER_OF_PROCESSES-1
            print ("  ")
            printInt (i)
            print (":")
            ThreadPrintShort (&threadTable[i])
          endFor
          print ("Here is the FREE list of Threads:\n   ")
          freeList.ApplyToEach (PrintObjectAddr)
          nl ()
          oldStatus = SetInterruptsTo (oldStatus)
      endMethod

      ----------  ThreadManager . GetANewThread  ----------

      method GetANewThread () returns ptr to Thread
        -- 
        -- This method returns a new Thread; it will wait
        -- until one is available.
        -- 

          var
              p1 : ptr to Thread 
          
          threadManagerLock.Lock()        -- aquire Monitor lock

            -- If freeLlist is empty then wait until thread become available
            while freeList.IsEmpty()
              aThreadBecomeFree.Wait(&threadManagerLock)    -- wait on CV
            endWhile

            -- If thread is available in freeList Remove it  
            p1 = freeList.Remove()
            p1.status = JUST_CREATED      -- change status to JUST_CREATED


          threadManagerLock.Unlock()      -- Release Monitor Lock

          return p1                       -- return thread ptr

      endMethod

      ----------  ThreadManager . FreeThread  ----------

      method FreeThread (th: ptr to Thread)
        -- 
        -- This method is passed a ptr to a Thread;  It moves it
        -- to the FREE list.
        -- 
          
          threadManagerLock.Lock()        -- aquire Monitor lock              

            if th
              th.status = UNUSED                              -- change status to UNUSED
              freeList.AddToEnd(th)                           -- add thread to freeList
              aThreadBecomeFree.Signal(&threadManagerLock)    -- signal to CV when thread become free
            endIf  

          threadManagerLock.Unlock()      -- Release Monitor lock


        endMethod

    endBehavior

--------------------------  ProcessControlBlock  ------------------------------

  behavior ProcessControlBlock

      ----------  ProcessControlBlock . Init  ----------
      --
      -- This method is called once for every PCB at startup time.
      --
      method Init ()
          pid = -1
          status = FREE
          addrSpace = new AddrSpace
          addrSpace.Init ()
-- Uncomment this code later...
/*
          fileDescriptor = new array of ptr to OpenFile
                      { MAX_FILES_PER_PROCESS of null }
*/
        endMethod

      ----------  ProcessControlBlock . Print  ----------

      method Print ()
        --
        -- Print this ProcessControlBlock using several lines.
        --
        -- var i: int
          self.PrintShort ()
          addrSpace.Print ()
          print ("    myThread = ")
          ThreadPrintShort (myThread)
-- Uncomment this code later...
/*
          print ("    File Descriptors:\n")
          for i = 0 to MAX_FILES_PER_PROCESS-1
            if fileDescriptor[i]
              fileDescriptor[i].Print ()
            endIf
          endFor
*/
          nl ()
        endMethod

      ----------  ProcessControlBlock . PrintShort  ----------

      method PrintShort ()
        --
        -- Print this ProcessControlBlock on one line.
        --
          print ("  ProcessControlBlock   (addr=")
          printHex (self asInteger)
          print (")   pid=")
          printInt (pid)
          print (", status=")
          if status == ACTIVE
            print ("ACTIVE")
          elseIf status == ZOMBIE
            print ("ZOMBIE")
          elseIf status == FREE
            print ("FREE")
          else
            FatalError ("Bad status in ProcessControlBlock")
          endIf
          print (", parentsPid=")
          printInt (parentsPid)
          print (", exitStatus=")
          printInt (exitStatus)
          nl ()
        endMethod

    endBehavior

-----------------------------  ProcessManager  ---------------------------------

  behavior ProcessManager

      ----------  ProcessManager . Init  ----------

      method Init ()
        --
        -- This method is called once at kernel startup time to initialize
        -- the one and only "processManager" object.  
        --
        var
          i : int
        
        processTable = new array of ProcessControlBlock { MAX_NUMBER_OF_PROCESSES of new ProcessControlBlock }  -- create array for processes
        freeList = new List[ProcessControlBlock]     -- create list for processes
        

        for i = 0 to MAX_NUMBER_OF_PROCESSES - 1
          processTable[i].Init()                      -- Initialize each process
          freeList.AddToEnd(& processTable[i])        -- add each process into freeList
        endFor  

        -- MutexLock and CV initialization
        processManagerLock = new Mutex
        processManagerLock.Init()
        aProcessBecameFree = new Condition
        aProcessBecameFree.Init()
        aProcessDied = new Condition
        aProcessDied.Init()


      endMethod

      ----------  ProcessManager . Print  ----------

      method Print ()
        -- 
        -- Print all processes.  Since we look at the freeList, this
        -- routine disables interrupts so the printout will be a
        -- consistent snapshot of things.
        -- 
        var i, oldStatus: int
          oldStatus = SetInterruptsTo (DISABLED)
          print ("Here is the process table...\n")
          for i = 0 to MAX_NUMBER_OF_PROCESSES-1
            print ("  ")
            printInt (i)
            print (":")
            processTable[i].Print ()
          endFor
          print ("Here is the FREE list of ProcessControlBlocks:\n   ")
          freeList.ApplyToEach (PrintObjectAddr)
          nl ()
          oldStatus = SetInterruptsTo (oldStatus)
        endMethod

      ----------  ProcessManager . PrintShort  ----------

      method PrintShort ()
        -- 
        -- Print all processes.  Since we look at the freeList, this
        -- routine disables interrupts so the printout will be a
        -- consistent snapshot of things.
        -- 
        var i, oldStatus: int
          oldStatus = SetInterruptsTo (DISABLED)
          print ("Here is the process table...\n")
          for i = 0 to MAX_NUMBER_OF_PROCESSES-1
            print ("  ")
            printInt (i)
            processTable[i].PrintShort ()
          endFor
          print ("Here is the FREE list of ProcessControlBlocks:\n   ")
          freeList.ApplyToEach (PrintObjectAddr)
          nl ()
          oldStatus = SetInterruptsTo (oldStatus)
        endMethod

      ----------  ProcessManager . GetANewProcess  ----------

      method GetANewProcess () returns ptr to ProcessControlBlock
        --
        -- This method returns a new ProcessControlBlock; it will wait
        -- until one is available.
        --

        var 
          p1 : ptr to ProcessControlBlock

          processManagerLock.Lock()                         -- aquire Monitor Lock

            -- if freeList is empty wait until one become available
            while freeList.IsEmpty()                        
              aProcessBecameFree.Wait(&processManagerLock)  -- wait on CV 
            endWhile

            p1 = freeList.Remove()                          -- reove from freeList
            p1.status = ACTIVE                              -- update status to ACTIVE

          processManagerLock.Unlock()                       -- Release Monitor lock

          return p1
      endMethod

      ----------  ProcessManager . FreeProcess  ----------

      method FreeProcess (p: ptr to ProcessControlBlock)
        --
        -- This method is passed a ptr to a Process;  It moves it
        -- to the FREE list.
        --
        
        processManagerLock.Lock()                          -- aquire Monitor Lock

          p.status = FREE                                  -- update Status to FREE
          freeList.AddToEnd(p)                             -- add process to freeList
          aProcessBecameFree.Signal(&processManagerLock)   -- Signal to CV

        processManagerLock.Unlock()                        -- Release Monitor lock

      endMethod


    endBehavior

-----------------------------  PrintObjectAddr  ---------------------------------

  function PrintObjectAddr (p: ptr to Object)
    --
    -- Print the address of the given object.
    --
      printHex (p asInteger)
      printChar (' ')
    endFunction

-----------------------------  ProcessFinish  --------------------------

  function ProcessFinish (exitStatus: int)
    --
    -- This routine is called when a process is to be terminated.  It will
    -- free the resources held by this process and will terminate the
    -- current thread.
    --
      FatalError ("ProcessFinish is not implemented")
    endFunction

-----------------------------  FrameManager  ---------------------------------

  behavior FrameManager

      ----------  FrameManager . Init  ----------

      method Init ()
        --
        -- This method is called once at kernel startup time to initialize
        -- the one and only "frameManager" object.  
        --
        var i: int
          print ("Initializing Frame Manager...\n")
          framesInUse = new BitMap
          framesInUse.Init (NUMBER_OF_PHYSICAL_PAGE_FRAMES)
          numberFreeFrames = NUMBER_OF_PHYSICAL_PAGE_FRAMES
          frameManagerLock = new Mutex
          frameManagerLock.Init ()
          newFramesAvailable = new Condition
          newFramesAvailable.Init ()
          waitingThreads = new Condition
          waitingThreads.Init ()
          -- Check that the area to be used for paging contains zeros.
          -- The BLITZ emulator will initialize physical memory to zero, so
          -- if by chance the size of the kernel has gotten so large that
          -- it runs into the area reserved for pages, we will detect it.
          -- Note: this test is not 100%, but is included nonetheless.
          for i = PHYSICAL_ADDRESS_OF_FIRST_PAGE_FRAME
                   to PHYSICAL_ADDRESS_OF_FIRST_PAGE_FRAME+300
                   by 4
            if 0 != *(i asPtrTo int)
              FatalError ("Kernel code size appears to have grown too large and is overflowing into the frame region")
            endIf
          endFor
        endMethod

      ----------  FrameManager . Print  ----------

      method Print ()
        --
        -- Print which frames are allocated and how many are free.
        --
          frameManagerLock.Lock ()
          print ("FRAME MANAGER:\n")
          printIntVar ("  numberFreeFrames", numberFreeFrames)
          print ("  Here are the frames in use: \n    ")
          framesInUse.Print ()
          frameManagerLock.Unlock ()
        endMethod

      ----------  FrameManager . GetAFrame  ----------

      method GetAFrame () returns int
        --
        -- Allocate a single frame and return its physical address.  If no frames
        -- are currently available, wait until the request can be completed.
        --
          var f, frameAddr: int

          -- Acquire exclusive access to the frameManager data structure...
          frameManagerLock.Lock ()

          -- Wait until we have enough free frames to entirely satisfy the request...
          while numberFreeFrames < 1
            newFramesAvailable.Wait (&frameManagerLock)
          endWhile

          -- Find a free frame and allocate it...
          f = framesInUse.FindZeroAndSet ()
          numberFreeFrames = numberFreeFrames - 1

          -- Unlock...
          frameManagerLock.Unlock ()

          -- Compute and return the physical address of the frame...
          frameAddr = PHYSICAL_ADDRESS_OF_FIRST_PAGE_FRAME + (f * PAGE_SIZE)
          -- printHexVar ("GetAFrame returning frameAddr", frameAddr)
          return frameAddr
        endMethod

      ----------  FrameManager . GetNewFrames  ----------

      method GetNewFrames (aPageTable: ptr to AddrSpace, numFramesNeeded: int)
          -- 
          -- This method allocate new frames and return its physical 
          -- adreess and stores it in to AddrSpace Object
          -- If no frames are awailable , wait until the request can be completed.
          --
          	var 
          	   i, f, frameAddr : int
               wait_Group : int = 0

          	frameManagerLock.Lock()  			-- aquire monitor lock

               wait_Group = wait_Group + 1      -- increment waiting group

               if wait_Group > 1                             
                waitingThreads.Wait(&frameManagerLock)             -- add in waiting group if other thread is already in execution
               endIf

          		while numberFreeFrames < numFramesNeeded	   -- check if requested frames are available or not
          		  newFramesAvailable.Wait(&frameManagerLock)   -- wait if requested frames are not available
          		endWhile

          		for i = 0 to numFramesNeeded - 1
          		  f = framesInUse.FindZeroAndSet()		-- determine free frame
          		  frameAddr = PHYSICAL_ADDRESS_OF_FIRST_PAGE_FRAME + (f * PAGE_SIZE)	-- get physical adreess of free frame
          		  aPageTable.SetFrameAddr(i, frameAddr)   -- store address of allocated frame 
          		endFor  

          		-- update number of free frames
          		numberFreeFrames = numberFreeFrames - numFramesNeeded

          		-- set pageTable to the number of allocated frames
          		aPageTable.numberOfPages = aPageTable.numberOfPages + numFramesNeeded

              	wait_Group = wait_Group - 1                   -- decrement waiting group
              	waitingThreads.Signal(&frameManagerLock)      -- signal waiting thread

          	frameManagerLock.Unlock() 			-- Release monitor lock	

      endMethod

      ----------  FrameManager . ReturnAllFrames  ----------

      method ReturnAllFrames (aPageTable: ptr to AddrSpace)
          -- 
          -- This method update number of free frames and signal waiting thread
          --

          var 
          	i, numFrameReturned, frameAddr, bitNumber : int

          	frameManagerLock.Lock() 			                         -- aquire monitor lock

          	 numFrameReturned = aPageTable.numberOfPages 	       	-- number of frame returned

          	 for i = 0 to numFrameReturned - 1
          	  frameAddr = aPageTable.ExtractFrameAddr (i)         
			  bitNumber = ( frameAddr -  PHYSICAL_ADDRESS_OF_FIRST_PAGE_FRAME ) / PAGE_SIZE   -- get Number of Bit to clear
			  framesInUse.ClearBit(bitNumber )                   	-- Clear Bit to 0
			  numberFreeFrames = numberFreeFrames + 1		         -- update number of free frames
		     endFor


             -- set pageTable 
             aPageTable.numberOfPages = aPageTable.numberOfPages - numFrameReturned

			 newFramesAvailable.Signal(&frameManagerLock)      -- signal that new frames are available


          	frameManagerLock.Unlock() 			                       -- Release monitor lock

      endMethod

    endBehavior



endCode
