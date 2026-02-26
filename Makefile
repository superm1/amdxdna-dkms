# amdxdna-dkms - Convenience Makefile

.PHONY: help update-sources build-deb clean install test

help:
	@echo "Available targets:"
	@echo "  help           - Show this help message"
	@echo "  update-sources - Sync driver and firmware from upstream"
	@echo "  build-deb      - Build Debian package"
	@echo "  clean          - Clean build artifacts"
	@echo "  install        - Install the built package (requires sudo)"
	@echo "  test           - Verify installation"

update-sources:
	./scripts/update-from-upstream.sh

build-deb:
	dpkg-buildpackage -us -uc -b

clean:
	dh clean
	rm -f ../*.deb ../*.ddeb ../*.build ../*.buildinfo ../*.changes

install: build-deb
	sudo dpkg -i ../amdxdna-dkms_*.deb

test:
	@echo "=== DKMS Status ==="
	dkms status amdxdna || true
	@echo ""
	@echo "=== Firmware Files ==="
	ls -lR /lib/firmware/updates/amdnpu/ 2>/dev/null || echo "Firmware not installed"
	@echo ""
	@echo "=== Module Info ==="
	modinfo amdxdna 2>/dev/null || echo "Module not loaded/available"
