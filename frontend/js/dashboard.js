
const userData = {
    profileName: "John Doe",
    availableEndpoints: [
        { name: "Endpoint 1", url: "#endpoint1" },
        { name: "Endpoint 2", url: "#endpoint2" },
        { name: "Endpoint 3", url: "#endpoint3" }
    ],
    favoriteFolders: [
        { name: "Folder 1", url: "#folder1" },
        { name: "Folder 2", url: "#folder2" },
        { name: "Folder 3", url: "#folder3" }
    ],
    files: [
        { name: "File 1.pdf", url: "#file1" },
        { name: "File 2.png", url: "#file2" },
        { name: "File 3.docx", url: "#file3" }
    ]
};

// Populate profile button
const profileButton = document.getElementById("profileButton");
profileButton.textContent = userData.profileName;

// Populate side menu
const sideMenu = document.getElementById("sideMenu");

function createMenuSection(title, items) {
    const section = document.createElement("section");
    section.classList.add("menu-section");

    const sectionTitle = document.createElement("h3");
    sectionTitle.textContent = title;
    section.appendChild(sectionTitle);

    const list = document.createElement("ul");
    items.forEach(item => {
        const listItem = document.createElement("li");
        const link = document.createElement("a");
        link.textContent = item.name;
        link.href = item.url;
        listItem.appendChild(link);
        list.appendChild(listItem);
    });
    section.appendChild(list);

    return section;
}

sideMenu.appendChild(createMenuSection("Available Endpoints", userData.availableEndpoints));
sideMenu.appendChild(createMenuSection("Favorite Folders", userData.favoriteFolders));

// Populate main content
// Populate file list
const mainContent = document.getElementById("mainContent");
const mainTitle = document.createElement("h1");
mainTitle.textContent = "Your Files";
mainContent.appendChild(mainTitle);

const fileList = document.createElement("div");
fileList.classList.add("file-list");

userData.files.forEach(file => {
  const fileItem = document.createElement("div");
  fileItem.classList.add("file-list-item");

  // File Info Section
  const fileInfo = document.createElement("div");
  fileInfo.classList.add("file-info");

  const fileIcon = document.createElement("div");
  fileIcon.classList.add("file-icon");
  fileIcon.textContent = file.name.split(".").pop().toUpperCase(); // File extension as icon

  const fileName = document.createElement("div");
  fileName.classList.add("file-name");
  fileName.textContent = file.name;

  fileInfo.appendChild(fileIcon);
  fileInfo.appendChild(fileName);

  // Actions Section
  const fileActions = document.createElement("div");
  fileActions.classList.add("file-actions");

  const openButton = document.createElement("button");
  openButton.textContent = "Open";
  openButton.onclick = () => {
    window.location.href = file.url;
  };

  const deleteButton = document.createElement("button");
  deleteButton.textContent = "Delete";
  deleteButton.onclick = () => {
    alert(`Delete ${file.name}?`);
  };

  fileActions.appendChild(openButton);
  fileActions.appendChild(deleteButton);

  // Combine sections into the file item
  fileItem.appendChild(fileInfo);
  fileItem.appendChild(fileActions);

  // Add to file list
  fileList.appendChild(fileItem);
});

mainContent.appendChild(fileList);