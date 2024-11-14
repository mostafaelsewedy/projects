package csen703.main.assignment1;

public class RingToss {

    public static int RingTossGreedy(int[] target) {

        if(target.length == 0){
            return 0;
        }

        int maxInRun = target[0];   
        int tosses = maxInRun;

        for (int i = 1; i < target.length ; i++) {
            if(target[i] == 0){
                maxInRun = target[i];
            }
            if(target[i] < maxInRun) {
                maxInRun = target[i];
            }
            if (target[i] > maxInRun) {
                tosses+= (target[i] - maxInRun);
                maxInRun = target[i];
            }
        }

        return tosses;
    }

    public static void main(String[] args) {
        int[] target = {};
        int result = RingTossGreedy(target);
        System.out.println("Minimum tosses needed: " + result);
    }
}


//Test Cases I used
// [2, 3, 4, 2, 1]                // [3, 2, 4, 2, 1, 0] 
// Minimum tosses needed: 4       // Minimum tosses needed: 5
// [6, 2, 0, 2, 0, 1]             // [2, 2, 2, 2, 2, 2, 2]
// Minimum tosses needed: 9       // Minimum tosses needed: 2
// [1, 2, 3, 4, 5, 6]             // [6, 5, 3, 2, 1]
// Minimum tosses needed: 6       // Minimum tosses needed: 6
// [1, 3, 5, 3, 1]                // [2, 1, 3, 2, 1]
// Minimum tosses needed: 5       // Minimum tosses needed: 4
// [4, 2, 1, 5, 0, 2]             // [1, 2, 1, 1, 0, 0]
// Minimum tosses needed: 10      // Minimum tosses needed: 2
// [4, 2, 5, 0, 2, 4]             // []
// Minimum tosses needed: 11      // Minimum tosses needed: 0