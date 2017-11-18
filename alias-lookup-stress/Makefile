PROJECT := alias-lookup-stress
BUILD_IMAGE := $(PROJECT)-build
SRCDIR := $(shell pwd)

all: buildc

.build-image: Dockerfile.build
	docker build -f Dockerfile.build . -t $(BUILD_IMAGE)
	date > .build-image

.PHONY: buildc
buildc: .build-image
	docker run \
		--rm \
		-u $$(id -u):$$(id -g) \
		-v $(SRCDIR)/:/src \
		$(BUILD_IMAGE) \
		/usr/bin/make -C /src build

.PHONY: build
build: stress

stress: stress.cc
	g++ -g stress.cc -o stress -pthread -static -Wall -Werror

.PHONY: clean
clean:
	rm -f .build-image
	rm -f stress
	docker rmi -f $(BUILD_IMAGE) || true
