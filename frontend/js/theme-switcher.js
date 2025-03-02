document.addEventListener('DOMContentLoaded', () => {
    const themeToggle = document.getElementById('theme-toggle');

    // Load stored theme preference
    const isDark = localStorage.getItem('theme') === 'dark';
    if (isDark) {
        document.body.classList.add('dark-theme');
        themeToggle.textContent = '🌙';
    }

    // Toggle theme on button click
    themeToggle.addEventListener('click', () => {
        const isDarkMode = document.body.classList.toggle('dark-theme');
        themeToggle.textContent = isDarkMode ? '🌞' : '🌙';
        localStorage.setItem('theme', isDarkMode ? 'dark' : 'light'); // Persist preference
    });
})