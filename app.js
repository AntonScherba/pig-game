/*
GAME RULES:

- The game has 2 players, playing in rounds
- In each turn, a player rolls a dice as many times as he whishes. Each result get added to his ROUND score
- BUT, if the player rolls a 1, all his ROUND score gets lost. After that, it's the next player's turn
- The player can choose to 'Hold', which means that his ROUND score gets added to his GLBAL score. After that, it's the next player's turn
- The first player to reach 100 points on GLOBAL score wins the game

*/

var score, roundScore, activePlayer, gamePlaying, previosValue, winningScore;

init();

document.querySelector('.btn-roll-1').addEventListener('click', () => {
  if (gamePlaying) {
    var dice = Math.floor(Math.random() * 6) + 1;
    
    var diceDOM = document.querySelector('.dice');
    diceDOM.style.display = 'block';
    diceDOM.src = 'dice-' + dice + '.png';        

    if (dice !== 1) {
      roundScore += dice;
      document.querySelector('#current-' + activePlayer).textContent = roundScore;

      var totalScore = previosValue.push(dice);
      
      if(previosValue[totalScore-1] == 6 && previosValue[totalScore-2] == 6) {
        console.log('doubleSix');
        previosValue = [];

        scores[activePlayer] = 0;

        document.querySelector('#score-' + activePlayer).textContent = scores[activePlayer];

        nextPlayer();
      }
      
    } else {
      previosValue = [];
      nextPlayer();
    } 
  }  
});

document.querySelector('.btn-hold').addEventListener('click', () => {
  if (gamePlaying) {
    previosValue = [];

    scores[activePlayer] += roundScore;

    document.querySelector('#score-' + activePlayer).textContent = scores[activePlayer];

    if (scores[activePlayer] >= winningScore) {
      document.querySelector('#name-' + activePlayer).textContent ='Winner!';
      document.querySelector('.dice').style.display = 'none';
      document.querySelector('.player-' + activePlayer + '-panel').classList.add('winner');
      document.querySelector('.player-' + activePlayer + '-panel').classList.remove('active');
      gamePlaying = false;
    } else {
      previosValue = [];
      nextPlayer();
    }  
  }  
});

document.querySelector('.btn-new').addEventListener('click', init);

document.querySelector('.btn-submit').addEventListener('click', () => {
  if (winningScore === 100) {
    winningScore = document.querySelector('.input-score').value;
    document.querySelector('.btn-submit').textContent = 'Winning score: ' + winningScore;
    console.log(document.querySelector('.input-score').value);  
    document.querySelector('.input-score').value = "";
  }
});



function init() {
  scores = [0, 0];
  activePlayer = 0;
  roundScore = 0;
  gamePlaying = true;
  previosValue = [];
  winningScore = 100;

  document.querySelector('.btn-submit').textContent = 'Submit winnig score'
    
  document.querySelector('.dice').style.display = 'none';

  document.getElementById('score-0').textContent = '0';
  document.getElementById('score-1').textContent = '0';
  document.getElementById('current-0').textContent = '0';
  document.getElementById('current-1').textContent = '0';
  document.getElementById('name-0').textContent = 'Player 1';
  document.getElementById('name-1').textContent = 'Player 2';
  document.querySelector('.player-0-panel').classList.remove('winner');
  document.querySelector('.player-1-panel').classList.remove('winner');
  document.querySelector('.player-0-panel').classList.remove('active');
  document.querySelector('.player-1-panel').classList.remove('active');
  document.querySelector('.player-0-panel').classList.add('active');  
}

function nextPlayer() {
  activePlayer === 0 ? activePlayer = 1 : activePlayer = 0;

      roundScore = 0;

      document.getElementById('current-0').textContent = '0';
      document.getElementById('current-1').textContent = '0';
      
      document.querySelector('.player-0-panel').classList.toggle('active');
      document.querySelector('.player-1-panel').classList.toggle('active');

      document.querySelector('.dice').style.display = 'none';
};




// document.querySelector('#current-' + activePlayer).textContent = dice; // TEXT
// document.querySelector('#current-' + activePlayer).innerHTML = '<em>' + dice + '</em>'; // HTML 

// var x = document.querySelector('#score-0').textContent;