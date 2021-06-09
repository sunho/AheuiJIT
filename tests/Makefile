.PHONY: std-tests perf-logo

std-tests:
	cd snippets && bash test.sh

perf-logo:
	time ${AHEUI} snippets/logo/logo.aheui > /dev/null

perf-aaquine:
	time ${AHEUI} aheui.aheui < snippets/quine/quine.puzzlet.40col.aheui > /dev/null

perf-aalogo:
	time ${AHEUI} aheui.aheui < snippets/logo/logo.aheui