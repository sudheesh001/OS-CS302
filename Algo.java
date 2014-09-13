import java.io.BufferedReader;
import java.io.FileReader;
import java.util.HashMap;
import java.util.Set;

import datastructures.INode;

public abstract class Algo {

        String filePathFormat;
        int fileStartIndex;
        int fileEndIndex;

        int numOfItems;
        int numOfBaskets;

        INode root;
        protected int k;
        protected long basketIndex;
        protected boolean foundSomething;

        protected int minK;
        protected int maxK;

        Set<String> alphabet;

        /***
         * One line of comma separated values = one basket of items. Line starting with "#" are treated as comments and thus
         * ignored. All the baskets are placed in several files according to the path from start to end. Comparison is case
         * insensitive!
         */
        public Algo(String _filePathFormat, int _fileStartIndex, int _fileEndIndex, Set<String> _alphabet) {
                filePathFormat = _filePathFormat;
                fileStartIndex = _fileStartIndex;
                fileEndIndex = _fileEndIndex;
                alphabet = _alphabet;
        }

        public Algo(String _filePathFormat, int _fileStartIndex, int _fileEndIndex, String _alphabetFilePath)
                        throws Exception {
                this(_filePathFormat, _fileStartIndex, _fileEndIndex, Common.loadAlphabetSetFromFile(_alphabetFilePath));
        }

        public Algo(String _filePathFormat, int _fileStartIndex, int _fileEndIndex, double support) throws Exception {
                this(_filePathFormat, _fileStartIndex, _fileEndIndex, Common.loadAlphabetSetFromInput(_filePathFormat,
                                _fileStartIndex, _fileEndIndex, support));
        }

        public Algo(String _filePathFormat, int _fileStartIndex, int _fileEndIndex) throws Exception {
                this(_filePathFormat, _fileStartIndex, _fileEndIndex, 0.0);
        }

        /***
         * Return the most frequent itemsets which are more frequent than the support and has more items (of type T) than
         * minimumK.
         * 
         * @param support
         *            - min frequency to consider items set frequent.
         * @param minimumK
         *            - min size of itemset to consider it interesting.
         * @return Map - for each K we keep the frequent K-itemsets.
         * @throws Exception
         */
        public INode findFrequentItemSet(double support, int minK, int maxK) throws Exception {

                BufferedReader br;
                String line;
                basketIndex = 0;
                iterNum = 0;
                k = 1;
                this.maxK = maxK;
                this.minK = minK;

                init();

                while (true) {
                        startOfSingleIteration();
                        for (int i = fileStartIndex; i <= fileEndIndex; i++) {
                                br = new BufferedReader(new FileReader(String.format(filePathFormat, i)));
                                while ((line = br.readLine()) != null) {
                                        if (line.startsWith("#") || line.equals("")) {
                                                // Ignore comments
                                                continue;
                                        }
                                        basketIndex++;
                                        if (!processSingleLine(line, support)) {
                                                return root;
                                        }
                                }
                                br.close();
                        }

                        if (!endOfSingleIteration(support, basketIndex, foundSomething)) {
                                break;
                        }
                }

                return root;
        }

        protected abstract void init();

        protected abstract void startOfSingleIteration();

        protected abstract boolean processSingleLine(String line, double support);

        protected abstract boolean endOfSingleIteration(double support, long basketIndex, boolean foundSomething);

        public int getNumOfItems() {
                return numOfItems;
        }

        public int getNumOfBaskets() {
                return numOfBaskets;
        }

        // STATS:
        private static class StatsHolder {
                int count;
        }

        private static HashMap<Integer, StatsHolder> levelToApperneces = new HashMap<Integer, StatsHolder>();
        private static int statsMaxK = 0;
        public static int iterNum = 0;

        public static synchronized void incStats(int k) {
                if (levelToApperneces.containsKey(k)) {
                        levelToApperneces.get(k).count++;
                } else {
                        if (k > statsMaxK)
                                statsMaxK = k;
                        StatsHolder tmp = new StatsHolder();
                        tmp.count = 1;
                        levelToApperneces.put(k, tmp);
                }
        }

        public static String getStats() {
                StringBuilder sb = new StringBuilder();

                for (int i = 1; i <= statsMaxK; i++) {
                        sb.append(String.format(" level - %d@items - %d\n", i, levelToApperneces.get(i).count));
                }

                return sb.toString();
        }

        public Set<String> getAlphabet() {
                return alphabet;
        }

        public void setAlphabet(Set<String> alphabet) {
                this.alphabet = alphabet;
        }

        @Override
        public String toString() {
                return this.getClass().getSimpleName();
        }
}