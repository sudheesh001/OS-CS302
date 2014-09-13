import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import datastructures.DicHashTreeNode;

public class Dic extends Algo {

        protected DicHashTreeNode hashTreeRoot;
        protected int M;
        protected int lastIter = Integer.MAX_VALUE;
        protected int estimatedNumOfBaskets;
        protected Map<Integer, Long> kDiscoveryIndices;
        protected int lastKWithDiscoveries;
        protected int maxBasketSize;

        public Dic(String pathFormat, int startIndex, int endIndex, int _M, Set<String> alphabetSet,
                        int _estimatedNumOfBaskets) throws Exception {
                super(pathFormat, startIndex, endIndex, alphabetSet);
                hashTreeRoot = new DicHashTreeNode(0, 0);
                root = hashTreeRoot;
                estimatedNumOfBaskets = _estimatedNumOfBaskets;
                kDiscoveryIndices = new HashMap<Integer, Long>();
                kDiscoveryIndices.put(0, 1L);
                kDiscoveryIndices.put(1, 1L);

                // Small inputs work around
                // If M is greater than the input size we will set it to the input size
                // and then the Algorithm will run like Apriori
                M = _M < estimatedNumOfBaskets ? _M : estimatedNumOfBaskets;
        }

        protected void init() {
                hashTreeRoot.depth = 0;
                hashTreeRoot.set = null;
                lastKWithDiscoveries = 0;
                maxBasketSize = 0;
        }

        protected void startOfSingleIteration() {
                foundSomething = false;
                iterNum++;
        }

        public boolean endOfSingleIteration(double support, long basketIndex, boolean foundSomething) {
                estimatedNumOfBaskets = numOfBaskets;

                return true;
        }

        protected void pruneAndDiscover(double support) {
                hashTreeRoot.pruneAtDepth(this, support, estimatedNumOfBaskets, k, basketIndex);
                if (iterNum == 1 || k < maxBasketSize) {
                        k++;
                        kDiscoveryIndices.put(k, basketIndex);
                }
                foundSomething = (hashTreeRoot.discoverTillDepth(this, support, estimatedNumOfBaskets, k, basketIndex));
        }

        /**
         * Return the threshold (natural number) which an Item must appear more than to be
         * considered as common. Since during the first iteration we don't know the number of
         * baskets in our run we will return 0 so no item will be missed
         * 
         * @param support
         *            - The relative support
         * @return - The support as integer
         */
        public int getSupportAsInt(double support, long discoveryIndex) {
                return (int) Math.floor(support * estimatedNumOfBaskets);
        }

        protected boolean processSingleLine(String line, double support) {
                String[] tmpBasket = Common.parseLineToBasket(line);

                if (iterNum == 1) {// Collect some stats
                        numOfBaskets++;
                        int basketSize = tmpBasket.length;
                        numOfItems += basketSize;
                        if (basketSize > maxBasketSize) {
                                maxBasketSize = basketSize;
                        }
                }

                handleBasket(tmpBasket, k, hashTreeRoot, basketIndex, support);
                if (basketIndex % M == 0) {
                        pruneAndDiscover(support);
                        if (!foundSomething) {
                                long prevKDiscoveryIndex = kDiscoveryIndices.get(lastKWithDiscoveries);
                                if (basketIndex - prevKDiscoveryIndex > estimatedNumOfBaskets) {
                                        return false;
                                }
                        } else {
                                lastKWithDiscoveries = k;
                        }
                }
                return true;
        }

        protected void handleBasket(String[] tmpBasket, int k, DicHashTreeNode root, long basketIndex, double support) {
                Arrays.sort(tmpBasket);
                int subsetsSize = k > tmpBasket.length ? tmpBasket.length : k;
                for (int i = 1; i <= subsetsSize; i++) {
                        if (estimatedNumOfBaskets == Integer.MAX_VALUE
                                        || basketIndex - kDiscoveryIndices.get(i) <= 2 * estimatedNumOfBaskets) {
                                handleOneBasketPhase(tmpBasket, i, root, basketIndex, support);
                        }
                }
        }

        protected void handleOneBasketPhase(String[] tmpBasket, int subsetsSize, DicHashTreeNode root, long basketIndex,
                        double support) {
                List<List<String>> subsets = Common.getAllSubsets(tmpBasket, subsetsSize, getAlphabet());
                String word;
                if (subsets == null)
                        return;
                DicHashTreeNode traverser;
                for (List<String> list : subsets) {
                        traverser = root;
                        if (traverser.childrens == null)
                                continue;
                        // assert(k==list.size());
                        for (int i = 0; i < subsetsSize; i++) {
                                word = list.get(i);
                                if (traverser.childrens.containsKey(word)) {
                                        traverser = traverser.childrens.get(word);
                                        if (i == subsetsSize - 1) {
                                                traverser.encounterNode(basketIndex, estimatedNumOfBaskets, getSupportAsInt(support, traverser.discoveryIndex),
                                                                iterNum);
                                        }
                                } else if (i == 0) {
                                        Algo.incStats(1);
                                        traverser = new DicHashTreeNode(1, basketIndex, word, root, iterNum, estimatedNumOfBaskets, support);
                                } else {
                                        break;
                                }
                        }
                }
        }
}