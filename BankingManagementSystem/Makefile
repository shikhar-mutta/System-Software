# Root Makefile for Banking Management System

all:
	@echo "🏗️  Building Server and Client..."
	$(MAKE) -C server
	$(MAKE) -C client

# clean:
c:
	@echo "🧹 Cleaning up..."
	$(MAKE) -C server clean
	$(MAKE) -C client clean

clean:
	@echo "🧹 Cleaning up..."
	$(MAKE) -C server clean
	$(MAKE) -C client clean

# run-server:
rs:
	@echo "🚀 Launching Server..."
	$(MAKE) -C server run

run-server:
	@echo "🚀 Launching Server..."
	$(MAKE) -C server run

# run-client:
rc:
	@echo "🚀 Launching Client..."
	$(MAKE) -C client run

run-client:
	@echo "🚀 Launching Client..."
	$(MAKE) -C client run


crs:
	@echo "🧹 Cleaning up..."
	$(MAKE) -C server clean
	$(MAKE) -C client clean
	@echo "🚀 Launching Server..."
	$(MAKE) -C server run
	@echo "🚀 Launching Client..."
	$(MAKE) -C client run
